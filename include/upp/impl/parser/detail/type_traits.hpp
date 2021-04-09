#pragma once
#include "upp/impl/parser/ast.hpp"

namespace upp{
namespace parser{
namespace detail{

template<class T>
struct is_ast: std::false_type{};

template<class Iter, class M, class OnMatch>
struct is_ast<Ast<Iter, M, OnMatch>>: std::true_type{};

template<class Iter, class OnMatch>
struct is_ast<DynAst<Iter, OnMatch>>: std::true_type{};

template<class T>
constexpr bool is_ast_v = is_ast<std::decay_t<T>>::value;

template<class T>
struct iter{};

template<class Iter, class M, class OnMatch>
struct iter<Ast<Iter, M, OnMatch>>{
	using type = Iter;
};

template<class Iter, class OnMatch>
struct iter<DynAst<Iter, OnMatch>>{
	using type = Iter;
};

template<class T>
using iter_t = typename iter<std::decay_t<T>>::type;

}
}
}
