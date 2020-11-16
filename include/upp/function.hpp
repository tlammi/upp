#pragma once
#include <algorithm>
#include <cstddef>
#include <new>
#include <stdexcept>
#include <iostream>

#include "upp/impl/traits/callable_prototype.hpp"

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


/**
 * \brief Generic callable template with statically allocated buffer
 *
 * Acts much like std::function except that upp::Function always uses
 * an internal buffer for object storage and never allocates dynamically.
 *
 * The wrapped object's size is either statically or dynamically asserted.
 * Direct assignation of a callable object, same Function specialization
 * and smaller Function specializations can fail only statically.
 * Functions with larger buffer size can be inserted into Functions with
 * smaller buffer sizes but an exception may be thrown if the contained
 * object is too large for the buffer of the target Function.
 *
 * \tparam C Wrapped callable type
 * \tparam Size Static buffer size
 * \tparam Align Static buffer alignment
*/
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
	typedef Ret(prototype_t)(Args...);

	Function(){}

	/**
	 * \brief Construct a Function wrapping the given object
	 *
	 * \param c Wrapped object
	 */
	template<typename C,
		typename = std::enable_if_t<!detail::is_function<C>::value>
		>
	explicit Function(C&& c){
		init(std::forward<C>(c));
	}
	/**
	 * \brief Assign a callable object
	 */
	template<typename C,
		typename = std::enable_if_t<!detail::is_function<C>::value>>
	Function& operator=(C&& c){
		init(std::forward<C>(c));
		return *this;
	}

	/**
	 * \brief Construct a function containing a function pointer
	 *
	 * \param func Function to wrap
	 */
	Function(Ret(*func)(Args...)){
		static_assert(sizeof(func) <= Size, "Object too large");
		invoke_ = &detail::do_invoke<Ret(*)(Args...), Ret, Args...>;
		*static_cast<Ret(**)(Args...)>(static_cast<void*>(blob_)) = func;
		size_ = sizeof(func);
	}

	Function(const Function&) = delete;
	Function& operator=(const Function&) = delete;

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

	/**
	 * \brief Move another Function specialization into the Function
	 *
	 * \throws std::runtime_error if the contained object is too large
	 */
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
		std::swap(size_, rhs.size_);
		std::swap(invoke_, rhs.invoke_);
		std::swap(dtor_, rhs.dtor_);
		std::copy(rhs.blob_, rhs.blob_ + size_, blob_);
	}
	
	/**
	 * \brief Assign another Function specialization into the Function
	 *
	 * \throws std::runtime_error if the contained object is too large
	 */
	template<
		typename OtherFunc,
		typename = std::enable_if_t<detail::is_function<OtherFunc>::value>,
		typename = std::enable_if_t<
			!std::is_same_v<OtherFunc, Function<Ret(Args...), Size, Align>>>
		>
	Function& operator=(OtherFunc&& rhs){
		if constexpr (OtherFunc::max_size > max_size)
			if(rhs.size_ > max_size)
				throw std::runtime_error("Function object too large");	
		size_ = 0;
		std::swap(size_, rhs.size_);
		std::swap(invoke_, rhs.invoke_);
		std::swap(dtor_, rhs.dtor_);
		std::copy(rhs.blob_, rhs.blob_ + size_, blob_);
		return *this;
	}


	template<
		typename OtherFunc,
		typename = std::enable_if_t<detail::is_function<OtherFunc>::value>,
		typename = std::enable_if_t<!std::is_same_v<
			OtherFunc, Function<Ret(Args...), Size, Align>>>
		>
	Function(const OtherFunc&) = delete;
	template<
		typename OtherFunc,
		typename = std::enable_if_t<detail::is_function<OtherFunc>::value>,
		typename = std::enable_if_t<!std::is_same_v<
			OtherFunc, Function<Ret(Args...), Size, Align>>>
		>
	Function& operator=(const OtherFunc&) = delete;

	~Function(){
		if(size_ && dtor_)
			dtor_(static_cast<void*>(blob_));
	}
		
	/// Invoke the contained object
	template<typename... Args2>
	Ret operator()(Args2... args){
		return invoke_(static_cast<void*>(blob_), std::forward<Args2>(args)...);
	}
	
	/// Currently used buffer size
	size_t used_size() const {
		return size_;
	}
	
private:
	
	/**
	 * \brief Initialize all required fields
	 */
	template<typename C>
	void init(C&& c){
		static_assert(sizeof(std::decay_t<C>) <= Size, "Object too large");
		invoke_ = &detail::do_invoke<std::decay_t<C>, Ret, Args...>;
		dtor_ = &detail::do_dtor<std::decay_t<C>>;
		new(static_cast<std::decay_t<C>*>(static_cast<void*>(blob_)))
			std::decay_t<C>(std::forward<C>(c));
		size_ = sizeof(std::decay_t<C>);
	}
	size_t size_{0};
	Ret(*invoke_)(void*, Args...){nullptr};
	void(*dtor_)(void*){nullptr};
	alignas(Align) char blob_[Size]{0,};
};


template<typename C,
	typename = std::enable_if_t<!std::is_lvalue_reference_v<C>>,
	typename = std::enable_if_t<!detail::is_function<C>::value>>
Function(C&&) -> Function<typename traits::callable_prototype<C>::type>;

template<typename C,
	typename = std::enable_if_t<!detail::is_function<C>::value>>
Function(const C&) -> Function<typename traits::callable_prototype<C>::type>;

}  // namespace upp
