#pragma once

#include <vector>
#include <deque>
#include "upp/impl/parser/detail/cbholder.hpp"

namespace upp{
namespace parser{

template<class>
class Ast;

namespace detail{

template<class Iter>
struct Match{
	Match(){}
	Match(const Ast<Iter>* ast, Iter begin, Iter end):
		ast{ast}, begin{begin}, end{end}{}

	const Ast<Iter>* ast;
	Iter begin;
	Iter end;
};

template<class Iter>
struct Ctx{
	using Skipper = Iter(*)(Iter, Iter);
	Iter iter;
	const Iter end;
	const Skipper skipper;
	std::deque<const Ast<Iter>*> expecteds{};
	std::deque<Match<Iter>> matches{};
};

template<class Iter, class OnMatch>
Iter* prepare_match(Ctx<Iter>& ctx, const Ast<Iter>* ast, OnMatch&& on_match){
	if(ctx.skipper) ctx.iter = ctx.skipper(ctx.iter, ctx.end);
	if constexpr (on_match){
		ctx.matches.emplace_back(ast, ctx.iter, ctx.end);
		return &ctx.matches.back().end;
	} else{
		(void)ast;
		return nullptr;
	}
}


template<class Iter>
void register_match(Ctx<Iter>& ctx, Iter* end, size_t inc){
	ctx.iter += inc;
	if(end) *end = ctx.iter;
}

template<class Iter>
void register_match_leaf(Ctx<Iter>& ctx, Iter* end, size_t inc){
	ctx.iter += inc;
	ctx.expecteds.clear();
	if(end) *end = ctx.iter;
}

template<class Iter>
void register_miss(Ctx<Iter>& ctx, Iter* end){
	if(end) ctx.matches.pop_back();
}

template<class Iter>
void register_miss_leaf(Ctx<Iter>& ctx, const Ast<Iter>* ast, Iter* end){
	if(end) ctx.matches.pop_back();
	ctx.expecteds.emplace_back(ast);
}

}
}
}
