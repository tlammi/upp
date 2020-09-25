#pragma once

#include <type_traits>
#include <utility>
namespace upp {
namespace traits {

template <typename T>
struct callable_traits : public callable_traits<decltype(&T::operator())> {};

template <typename Ret, typename... Args>
struct callable_traits<Ret (*)(Args...)> {
		using ret_t = Ret;
		using arg_tuple_t = std::tuple<Args...>;
};

template <typename Ret, typename Class, typename... Args>
struct callable_traits<Ret (Class::*)(Args...) const> {
		using ret_t = Ret;
		using arg_tuple_t = std::tuple<Args...>;
};
template <typename Ret, typename Class, typename... Args>
struct callable_traits<Ret (Class::*)(Args...)> {
		using ret_t = Ret;
		using arg_tuple_t = std::tuple<Args...>;
};
}  // namespace traits
}  // namespace upp