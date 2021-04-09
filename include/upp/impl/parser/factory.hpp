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

	template<class OnMatch>
	Ast<Iter, Regex<Iter>, OnMatch> regex(std::string_view re, OnMatch&& on_match) const {
		return {Regex<Iter>{re}, std::forward<OnMatch>(on_match)};
	}

	Ast<Iter, Regex<Iter>> regex(std::string_view re) const {
		return {Regex<Iter>{re}};
	}


	template<class OnMatch>
	Ast<Iter, LiteralChar<Iter>, OnMatch> lit(char c, OnMatch&& on_match) const {
		return {LiteralChar<Iter>{c}, std::forward<OnMatch>(on_match)};
	}

	Ast<Iter, LiteralChar<Iter>> lit(char c) const {
		return {LiteralChar<Iter>{c}};
	}

	template<class OnMatch>
	Ast<Iter, LiteralStr<Iter>, OnMatch> lit(std::string_view str, OnMatch&& on_match) const {
		return {LiteralStr<Iter>{str}, std::forward<OnMatch>(on_match)};
	}

	Ast<Iter, LiteralStr<Iter>> lit(std::string_view str) const {
		return {LiteralStr<Iter>{str}};
	}

private:
};

template<class Iter>
Factory<Iter> factory(Iter){
	return {};
}
}
}
