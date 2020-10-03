#pragma once
#include <algorithm>
#include <cstddef>
#include <new>
#include <stdexcept>

#include "upp/impl/traits/callable_traits.hpp"
namespace upp {
namespace detail {

template <typename Ret, typename ArgTuple>
struct Callable {};

template <typename Ret, typename... Args>
struct Callable<Ret, std::tuple<Args...>> {
		virtual ~Callable() {}
		virtual Ret operator()(Args...) = 0;
};

template <typename C, typename Ret, typename ArgTuple>
struct CallableImpl {};

template <typename C, typename Ret, typename... Args>
struct CallableImpl<C, Ret, std::tuple<Args...>>
	: Callable<Ret, std::tuple<Args...>> {
		CallableImpl(C&& c) : c_{c} {}
		Ret operator()(Args... args) final {
				return c_(std::forward<Args>(args)...);
		}
		C c_;
};

constexpr size_t FUNCTION_SIZE_RESOLUTION = 32;

}  // namespace detail

template <typename C, size_t S = detail::FUNCTION_SIZE_RESOLUTION>
struct Function {};

namespace detail {
template <typename T>
struct is_function : public std::false_type {};

template <typename Ret, size_t S, typename... Args>
struct is_function<Function<Ret(Args...), S>> : std::true_type {};
}  // namespace detail

template <typename Ret, size_t S, typename... Args>
class Function<Ret(Args...), S> {
		template <typename C, size_t S2>
		friend class Function;

public:
		static_assert(S % detail::FUNCTION_SIZE_RESOLUTION == 0,
					  "upp::Function size should be a multiple of 32");

		Function() {}

		template <typename C, typename Enable = std::enable_if_t<
								  !detail::is_function<C>::value>>
		Function(C&& c) {
				constexpr size_t size =
					sizeof(detail::CallableImpl<C, Ret, std::tuple<Args...>>);
				static_assert(size <= sizeof(buf_),
							  "Object too large to store in Func");
				size_ = size;
				new ((void*)buf_)
					detail::CallableImpl<C, Ret, std::tuple<Args...>>(
						std::forward<C>(c));
		}

		Function(const Function&) = default;
		Function(Function&&) noexcept = default;

		template <size_t S2, typename Enable = std::enable_if_t<S != S2>>
		explicit Function(const Function<Ret(Args...), S2>& rhs) {
				if constexpr (S < S2) {
						if (rhs.size_ > S)
								throw std::runtime_error("Object too large");
				}
				if (rhs.size_ != 0)
						std::copy(rhs.buf_, rhs.buf_ + rhs.size_, buf_);
				size_ = rhs.size_;
		}

		template <size_t S2, typename Enable = std::enable_if_t<S != S2>>
		explicit Function(Function<Ret(Args...), S2>&& rhs) {
				if constexpr (S < S2) {
						if (rhs.size_ > S)
								throw std::runtime_error("Object too large");
				}
				if (rhs.size_ != 0)
						std::copy(rhs.buf_, rhs.buf_ + rhs.size_, buf_);
				size_ = rhs.size_;
				rhs.size_ = 0;
		}

		~Function() {
				if (size_) {
						detail::Callable<Ret, std::tuple<Args...>>* ptr =
							(detail::Callable<Ret, std::tuple<Args...>>*)buf_;
						ptr->~Callable();
				}
		}

		Function& operator=(const Function&) = default;
		Function& operator=(Function&&) noexcept = default;

		template <size_t S2, typename Enable = std::enable_if_t<S != S2>>
		Function& operator=(const Function<Ret(Args...), S2>& rhs) {
				if constexpr (S < S2) {
						if (rhs.size_ > S)
								throw std::runtime_error("Object too large");
				}
				if (rhs.size_ != 0)
						std::copy(rhs.buf_, rhs.buf_ + rhs.size_, buf_);
				size_ = rhs.size_;
				return *this;
		}

		template <size_t S2, typename Enable = std::enable_if_t<S != S2>>
		Function& operator=(Function<Ret(Args...), S2>&& rhs) {
				if constexpr (S < S2) {
						if (rhs.size_ > S)
								throw std::runtime_error("Object too large");
				}
				if (rhs.size_ != 0)
						std::copy(rhs.buf_, rhs.buf_ + rhs.size_, buf_);
				size_ = rhs.size_;
				rhs.size_ = 0;
				return *this;
		}

		Ret operator()(Args... args) {
				if (!size_) throw std::runtime_error("Func::operator()");
				detail::Callable<Ret, std::tuple<Args...>>* ptr =
					(detail::Callable<Ret, std::tuple<Args...>>*)buf_;
				return (*ptr)(std::forward<Args>(args)...);
		}

		Ret operator()(Args... args) const {
				if (!size_) throw std::runtime_error("Func::operator()");
				const detail::Callable<Ret, std::tuple<Args...>>* ptr =
					(const detail::Callable<Ret, std::tuple<Args...>>*)buf_;
				return (*ptr)(std::forward<Args>(args)...);
		}

		explicit operator bool() const { return size_; }

private:
		size_t size_{0};
		alignas(alignof(size_t)) char buf_[S] = {0};
};

namespace detail {

template <typename C>
struct function_deducer {};

template <typename Ret, typename Class, typename... Args>
struct function_deducer<Ret (Class::*)(Args...) const> {
		using type = Ret(Args...);
};
template <typename Ret, typename Class, typename... Args>
struct function_deducer<Ret (Class::*)(Args...)> {
		using type = Ret(Args...);
};
}  // namespace detail

template <typename Ret, typename... Args>
Function(Ret (*)(Args...)) -> Function<Ret(Args...)>;

template <typename C>
Function(C) -> Function<
	typename detail::function_deducer<decltype(&C::operator())>::type>;

template <typename C>
auto make_function(C&& c) {
		return Function<
			typename detail::function_deducer<decltype(&C::operator())>::type,
			((sizeof(c) + 31) / 32) * 32>(std::forward<C>(c));
}

}  // namespace upp