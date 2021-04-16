#pragma once

#include "upp/impl/parser/regex.hpp"
#include "upp/impl/parser/ast.hpp"
#include "upp/impl/parser/lit.hpp"


namespace upp{
namespace parser{

template<class Iter>
class Factory{
public:
	DynAst<Iter> dyn_ast(){
		return {nullptr};
	}

	template<class Lhs, class Rhs, class OnMatch0, class OnMatch1>
	Joined<Iter, Lhs, Rhs, OnMatch0> ast(Joined<Iter, Lhs, Rhs, OnMatch1>&& other, OnMatch0&& on_match){
		return {std::move(other), std::forward<OnMatch0>(on_match)};
	}

	/*
	template<class M, class OnMatch0, class OnMatch1>
	Ast<Iter, M, OnMatch1> ast(Ast<Iter, M, OnMatch0>& a, OnMatch1&& m){
		return {a.matcher_, std::forward<OnMatch1>(m)};
	}

	template<class M, class OnMatch0, class OnMatch1>
	Ast<Iter, M, OnMatch1> ast(Ast<Iter, M, OnMatch0>&& a, OnMatch1&& m){
		return {std::move(a.matcher_), std::forward<OnMatch1>(m)};
	}
	*/

	template<class OnMatch>
	Regex<Iter, OnMatch> regex(std::string_view re, OnMatch&& on_match) const {
		return {re, std::forward<OnMatch>(on_match)};
	}

	Regex<Iter> regex(std::string_view re) const {
		return {re};
	}

	template<class OnMatch>
	LiteralChar<Iter, OnMatch> lit(char c, OnMatch&& on_match) const {
		return {c, std::forward<OnMatch>(on_match)};
	}

	LiteralChar<Iter> lit(char c) const {
		return LiteralChar<Iter>{c};
	}

	template<class OnMatch>
	LiteralStr<Iter, OnMatch> lit(std::string_view str, OnMatch&& on_match) const {
		return {str, std::forward<OnMatch>(on_match)};
	}

	LiteralStr<Iter> lit(std::string_view str) const {
		return {str};
	}

private:
};

template<class Iter>
Factory<Iter> factory(Iter){
	return {};
}
}
}
