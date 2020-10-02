#pragma once
#include <new>

#include "upp/impl/traits/callable_traits.hpp"
namespace upp {
namespace detail {

template <typename Ret, typename ArgTuple>
struct Callable {};

template <typename Ret, typename... Args>
struct Callable<Ret, std::tuple<Args...>> {
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
struct Func<Ret(Args...)> {
		template <typename C>
		Func(C&& c) {
				new ((void*)buf)
					detail::CallableImpl<C, Ret, std::tuple<Args...>>(
						std::move(c));
		}

		Ret operator()(Args... args) {
				detail::Callable<Ret, std::tuple<Args...>>* ptr =
					(detail::Callable<Ret, std::tuple<Args...>>*)buf;
				return (*ptr)(std::forward<Args>(args)...);
		}

		char buf[32];
};
}  // namespace upp