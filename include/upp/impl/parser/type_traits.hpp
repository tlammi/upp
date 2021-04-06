#pragma once
#include "upp/impl/parser/ast.hpp"

namespace upp{
namespace parser{

template<class T>
struct is_ast: std::false_type{};

template<class Iter, class M, class OnMatch>
struct is_ast<Ast<Iter, M, OnMatch>>: std::true_type{};

template<class T>
constexpr bool is_ast_v = is_ast<T>::value;

}
}
