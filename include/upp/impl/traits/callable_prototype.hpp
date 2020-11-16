/* SPDX-License-Identifier: MIT */
/* Copyright @ 2020 Toni Lammi */
#pragma once

#include <type_traits>
#include <utility>
namespace upp {
namespace traits {

template<typename C>
struct callable_prototype: public callable_prototype<decltype(&C::operator())>{};

template<typename Ret, typename... Args>
struct callable_prototype<Ret(*)(Args...)>{
	typedef Ret(type)(Args...);
};

template<typename Ret, typename... Args>
struct callable_prototype<Ret(Args...)>{
	typedef Ret(type)(Args...);
};

template<typename Ret, typename Class, typename... Args>
struct callable_prototype<Ret(Class::*)(Args...)>{
	typedef Ret(type)(Args...);
};

template<typename Ret, typename Class, typename... Args>
struct callable_prototype<Ret(Class::*)(Args...) const> {
	typedef Ret(type)(Args...);
};


}  // namespace traits
}  // namespace upp
