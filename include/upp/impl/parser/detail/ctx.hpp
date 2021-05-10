#pragma once

#include <vector>
#include <deque>
#include "upp/impl/parser/detail/cbholder.hpp"

namespace upp{
namespace parser{

template<class>
class Ast;

namespace detail{

/// Store arguments and pointers to invoke callbacks after parsing
template<class Iter>
struct Match{
	Match(){}
	Match(const Ast<Iter>* ast, Iter begin, Iter end):
		ast{ast}, begin{begin}, end{end}{}

	const Ast<Iter>* ast;
	Iter begin;
	Iter end;
};


/// Working context modified by all Asts
template<class Iter>
struct Ctx{
	using Skipper = Iter(*)(Iter, Iter);
	Iter iter; ///< Working position
	const Iter end; ///< End iterator for parsing
	const Skipper skipper; ///< Used for skipping ignored characters

	Iter max_iter{iter}; ///< Max value ever stored in \ref iter

	/// Currently failed ASTs. Used for finding which ASTs failed a match
	std::deque<const Ast<Iter>*> expecteds{};

	/// Matched ASTs. Used for invoking match callbacks
	std::deque<Match<Iter>> matches{};
};

/**
 * Utility to init Ctx before the actual match
 *
 * Skips ignored characters using skipper function and appends match callback
 * and its arguments to a deque. Callbacks are registered before the actual
 * match so callbacks are invoked in parent -> children order.
 *
 * \param ctx Parsing context
 * \param ast Pointer to current Ast instance
 * \param on_match CbHolder<T> holding the match callback
 *
 * \return Pointer to end parameter for setting after match or nullptr if no calblack is configured
 */
template<class Iter, class OnMatch>
Iter* prepare_match(Ctx<Iter>& ctx, const Ast<Iter>* ast, OnMatch&& on_match){
	if(ctx.skipper) ctx.iter = ctx.skipper(ctx.iter, ctx.end);
	if(ctx.iter > ctx.max_iter) ctx.max_iter = ctx.iter;
	if (on_match){
		ctx.matches.emplace_back(ast, ctx.iter, ctx.end);
		return &ctx.matches.back().end;
	} else{
		(void)ast;
		return nullptr;
	}
}

/**
 * Register current Ast as matched in Ctx
 *
 * Used for non-leaf Asts.
 *
 * \param ctx Parsing context
 * \param end Pointer returned by \ref prepare_match 
 * \param inc Increment applied to the iterator stored in Ctx
 */
template<class Iter>
void register_match(Ctx<Iter>& ctx, Iter* end, size_t inc){
	ctx.iter += inc;
	if(ctx.iter > ctx.max_iter) ctx.max_iter = ctx.iter;
	if(end) *end = ctx.iter;
}

/**
 * Register current (leaf) Ast as matched in Ctx
 *
 * \param ctx Parsing context
 * \param Iter \ref prepare_match return value
 * \param inc Increment applied to iterator
 */
template<class Iter>
void register_match_leaf(Ctx<Iter>& ctx, Iter* end, size_t inc){
	ctx.iter += inc;
	if(ctx.iter > ctx.max_iter){
		ctx.max_iter = ctx.iter;
		ctx.expecteds.clear();
	}
	if(end) *end = ctx.iter;
}

/**
 * Register current Ast as missed in the Ctx
 *
 * \param ctx Parsing context
 * \param end \ref prepre_match return value
 */
template<class Iter>
void register_miss(Ctx<Iter>& ctx, Iter* end){
	if(end) ctx.matches.pop_back();
}

/**
 * Register current (leaf) Ast as missed in Ctx
 *
 * \param ctx Parsing context
 * \param ast Current Ast
 * \param end \ref prepare_match return value
 */
template<class Iter>
void register_miss_leaf(Ctx<Iter>& ctx, const Ast<Iter>* ast, Iter* end){
	if(end) ctx.matches.pop_back();
	if(ctx.iter == ctx.max_iter) ctx.expecteds.emplace_back(ast);
}

/// Used in \ref Joined to restore iterator position if the second matcher fails
template<class Iter>
void register_miss_joined(Ctx<Iter>& ctx, Iter orig_begin, Iter* end){
	if(end) ctx.matches.pop_back();
	ctx.iter = orig_begin;
}

}
}
}
