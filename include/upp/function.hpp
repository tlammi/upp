#pragma once
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
}  // namespace detail

template <typename C>
struct Func {};

template <typename Ret, typename... Args>
class Func<Ret(Args...)> {
public:
		Func() {}
		template <typename C>
		Func(C&& c) : valid_{true} {
				static_assert(sizeof(c) <= sizeof(buf_),
							  "Object too large to store in Func");
				new ((void*)buf_)
					detail::CallableImpl<C, Ret, std::tuple<Args...>>(
						std::forward<C>(c));
		}

		Func(const Func&) = default;
		Func(Func&&) noexcept = default;

		~Func() {
				if (valid_) {
						detail::Callable<Ret, std::tuple<Args...>>* ptr =
							(detail::Callable<Ret, std::tuple<Args...>>*)buf_;
						ptr->~Callable();
				}
		}

		Func& operator=(const Func&) = default;
		Func& operator=(Func&&) noexcept = default;

		Ret operator()(Args... args) {
				if (!valid_) throw std::runtime_error("Func::operator()");
				detail::Callable<Ret, std::tuple<Args...>>* ptr =
					(detail::Callable<Ret, std::tuple<Args...>>*)buf_;
				return (*ptr)(std::forward<Args>(args)...);
		}

		Ret operator()(Args... args) const {
				if (!valid_) throw std::runtime_error("Func::operator()");
				const detail::Callable<Ret, std::tuple<Args...>>* ptr =
					(const detail::Callable<Ret, std::tuple<Args...>>*)buf_;
				return (*ptr)(std::forward<Args>(args)...);
		}

		explicit operator bool() const { return valid_; }

private:
		char buf_[32] = {0};
		bool valid_ = false;
};
}  // namespace upp