#pragma once

#include "upp/impl/parser/regex.hpp"
#include "upp/impl/parser/ast.hpp"
#include "upp/impl/parser/lit.hpp"
#include "upp/impl/parser/joined.hpp"


namespace upp{
namespace parser{

template<class Iter>
class Factory{
public:
	DynAst<Iter> dyn_ast(){
		return {nullptr};
	}
	
	template<class OnMatch>
	DynAst<Iter, OnMatch> dyn_ast(OnMatch&& on_match){
		return {std::forward<OnMatch>(on_match)};
	}

	template<template<class...> class T, class OnMatch, class... Ts>
	auto ast(T<Ts...>&& other, OnMatch&& on_match){
		return T{std::move(other), std::forward<OnMatch>(on_match)};
	}

	template<template<class...> class T, class OnMatch, class... Ts>
	auto ast(const T<Ts...>& other, OnMatch&& on_match){
		return T{other, std::forward<OnMatch>(on_match)};
	}

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
