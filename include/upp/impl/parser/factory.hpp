#pragma once

#include "upp/impl/parser/regex.hpp"
#include "upp/impl/parser/ast.hpp"
#include "upp/impl/parser/lit.hpp"
#include "upp/impl/parser/joined.hpp"


namespace upp{
namespace parser{

/**
 * Class for constructing objects in parser namespace
 *
 * This class is provided for simplicity. Factory<Iter> stores the Iter type
 * so it does not have to be specified all of the time.
 */
template<class Iter>
class Factory{
public:
	/**
	 * Create empty DynAst
	 *
	 * Used mainly as a "forward declaration" of an Ast so a recursive
	 * matching rule can be created.
	 */
	DynAst<Iter> dyn_ast(){
		return {nullptr};
	}
	
	/**
	 * Create empty DynAst with match callback.
	 * 
	 * See \ref dyn_ast()
	 */
	template<class OnMatch>
	DynAst<Iter, OnMatch> dyn_ast(OnMatch&& on_match){
		return {std::forward<OnMatch>(on_match)};
	}

	/**
	 * Construct a new Ast replacing the callback
	 *
	 * This is an utility method to replace the match callback.
	 */
	template<template<class...> class T, class OnMatch, class... Ts>
	auto ast(T<Ts...>&& other, OnMatch&& on_match){
		return T{std::move(other), std::forward<OnMatch>(on_match)};
	}

	/**
	 * Construct a new Ast by copy and replacing the callback
	 */
	template<template<class...> class T, class OnMatch, class... Ts>
	auto ast(const T<Ts...>& other, OnMatch&& on_match){
		return T{other, std::forward<OnMatch>(on_match)};
	}

	/**
	 * Construct a new Ast replacing the callback
	 *
	 * This is an utility method to replace the match callback.
	 */
	template<template<class...> class T, class... Ts>
	auto ast(T<Ts...>&& other){
		return T{std::move(other)};
	}

	/**
	 * Construct a new Ast by copy and replacing the callback
	 */
	template<template<class...> class T, class... Ts>
	auto ast(const T<Ts...>& other){
		return T{other};
	}

	/**
	 * Construct a regex Ast
	 *
	 * \param re Regex used for matching lexemes in the input sequence
	 * \param name Verbose name used in error printing
	 * \param on_match Callback invoked on matches.
	 * @{
	 */
	template<class OnMatch>
	Regex<Iter, OnMatch> regex(std::string_view re, OnMatch&& on_match) const {
		return {re, std::forward<OnMatch>(on_match)};
	}

	template<class OnMatch>
	Regex<Iter, OnMatch> regex(std::string_view re, std::string_view name, OnMatch&& on_match) const {
		return {re, name, std::forward<OnMatch>(on_match)};
	}

	Regex<Iter> regex(std::string_view re) const {
		return {re};
	}

	Regex<Iter> regex(std::string_view re, std::string_view name) const {
		return {re, name};
	}
	/// @}

	/**
	 * Construct a character matching literal
	 * @{
	 */
	template<class OnMatch>
	LiteralChar<Iter, OnMatch> lit(char c, OnMatch&& on_match) const {
		return {c, std::forward<OnMatch>(on_match)};
	}

	LiteralChar<Iter> lit(char c) const {
		return LiteralChar<Iter>{c};
	}
	/// @}

	/**
	 * Construt a string matching literal
	 *
	 * @{
	 */
	template<class OnMatch>
	LiteralStr<Iter, OnMatch> lit(std::string_view str, OnMatch&& on_match) const {
		return {str, std::forward<OnMatch>(on_match)};
	}

	LiteralStr<Iter> lit(std::string_view str) const {
		return {str};
	}
	/// @}
};

/**
 * Utility for creating a factory
 *
 * \param Iter Used only for type deduction.
 */
template<class Iter>
Factory<Iter> factory(Iter){
	return {};
}
}
}
