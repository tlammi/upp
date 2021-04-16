#pragma once
#include "upp/impl/parser/ast.hpp"

namespace upp{
namespace parser{
namespace detail{

template<class F, class... Args>
struct first_of {
	using type = F;
};


template<class T>
struct first_tparam{};

template<template<class...> class T, class... Args>
struct first_tparam<T<Args...>>{
	using type = typename first_of<Args...>::type;
};



template<class T>
struct is_ast{
	using FirstTParam = typename first_tparam<T>::type;
	static constexpr bool value = std::is_base_of_v<Ast<FirstTParam>, T>;
};

template<class T>
constexpr bool is_ast_v = is_ast<std::decay_t<T>>::value;

template<class T>
using iter_t = typename first_tparam<std::decay_t<T>>::type;
}
}
}
