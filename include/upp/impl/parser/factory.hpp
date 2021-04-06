#pragma once

#include "upp/impl/parser/regex.hpp"
#include "upp/impl/parser/ast.hpp"


namespace upp{
namespace parser{

template<class Iter>
class Factory{
public:
	DynAst<Iter> dyn_ast(){
		return {};
	}

	template<class OnMatch>
	Ast<Iter, Regex<Iter>, OnMatch> regex(std::string_view re, OnMatch&& on_match){
		return {Regex<Iter>{re}, std::forward<OnMatch>(on_match)};
	}

	Ast<Iter, Regex<Iter>> regex(std::string_view re){
		return {Regex<Iter>{re}};
	}
private:
};

template<class Iter>
Factory<Iter> factory(Iter){
	return {};
}
}
}
