#pragma once
#include <algorithm>
#include <cstddef>
#include <new>
#include <stdexcept>

#include "upp/impl/traits/callable_traits.hpp"

namespace upp {
namespace detail {

template<typename C, typename R, typename... A>
R do_invoke(void* blob, A... args){
	return (*static_cast<C*>(blob))(std::forward<A>(args)...);
}


template<typename C>
void do_dtor(void* blob){
	static_cast<C*>(blob)->~C();
}

}// namespace detail

template<typename C, size_t Size=32, size_t Align=alignof(int)>
class Function{};

namespace detail {
template <typename T>
struct is_function : public std::false_type {};

template <typename Ret, size_t S, size_t A, typename... Args>
struct is_function<Function<Ret(Args...), S, A>> : std::true_type {};

}  // namespace detail

template<typename Ret, size_t Size, size_t Align, typename... Args>
class Function<Ret(Args...), Size, Align>{

	template<typename C, size_t Size2, size_t Align2>
	friend class Function;
public:
	using return_type = Ret;
	static constexpr size_t max_size = Size;

	Function(){}

	template<typename C,
		typename = std::enable_if_t<!detail::is_function<C>::value>>
	Function(C&& c){
		static_assert(sizeof(c) <= Size, "Object too large");
		invoke_ = &detail::do_invoke<C, Ret, Args...>;
		dtor_ = &detail::do_dtor<C>;
		new(static_cast<C*>(static_cast<void*>(blob_)))C(std::forward<C>(c));
		size_ = sizeof(c);
	}

	Function(Ret(*func)(Args...)){
		static_assert(sizeof(func) <= Size, "Object too large");
		invoke_ = &detail::do_invoke<Ret(*)(Args...), Ret, Args...>;
		*static_cast<Ret(**)(Args...)>(static_cast<void*>(blob_)) = func;
		size_ = 0;
	}

	Function(const Function&) = default;
	Function& operator=(const Function&) = default;
	Function(Function&& rhs) noexcept {
		std::swap(size_, rhs.size_);
		std::swap(invoke_, rhs.invoke_);
		std::swap(dtor_, rhs.dtor_);
		std::swap(blob_, rhs.blob_);
	}

	Function& operator=(Function&& rhs) noexcept {
		size_ = 0;
		std::swap(size_, rhs.size_);
		std::swap(invoke_, rhs.invoke_);
		std::swap(dtor_, rhs.dtor_);
		std::swap(blob_, rhs.blob_);
		return *this;
	}

	template<
		typename OtherFunc,
		typename = std::enable_if_t<detail::is_function<OtherFunc>::value>,
		typename = std::enable_if_t<
			!std::is_same_v<OtherFunc, Function<Ret(Args...), Size, Align>>>
		>
	Function(OtherFunc&& rhs){
		if constexpr (OtherFunc::max_size > max_size)
			if(rhs.size_ > max_size)
				throw std::runtime_error("Function object too large");
		
		size_ = 0;
		std::swap(size_, rhs.size_);
		std::swap(invoke_, rhs.invoke_);
		std::swap(dtor_, rhs.dtor_);
		std::copy(rhs.blob_, rhs.blob_ + size_, blob_);
	}

	template<
		typename OtherFunc,
		typename = std::enable_if_t<detail::is_function<OtherFunc>::value>,
		typename = std::enable_if_t<!std::is_same_v<
			OtherFunc, Function<Ret(Args...), Size, Align>>>
		>
	Function(const OtherFunc&) = delete;

	~Function(){
		if(size_ && dtor_)
			dtor_(static_cast<void*>(blob_));
	}


	template<typename... Args2>
	Ret operator()(Args2... args){
		return invoke_(static_cast<void*>(blob_), std::forward<Args2>(args)...);
	}
	
private:
	size_t size_{0};
	Ret(*invoke_)(void*, Args...){nullptr};
	void(*dtor_)(void*){nullptr};
	alignas(Align) char blob_[Size]{0,};
};

}  // namespace upp
