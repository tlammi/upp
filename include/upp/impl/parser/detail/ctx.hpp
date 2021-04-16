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

template<class Iter>
Iter& prepare_match(Ctx<Iter>& ctx, const Ast<Iter>* ast){
	ctx.matches.emplace_back(ast, ctx.iter, ctx.end);
	if(ctx.skipper) ctx.iter = ctx.skipper(ctx.iter, ctx.end);
	return ctx.matches.back().end;
}


template<class Iter>
void register_match(Ctx<Iter>& ctx, Iter& end, size_t inc){
	ctx.iter += inc;
	ctx.expecteds.clear();
	end = ctx.iter;
}


template<class Iter>
void register_miss(Ctx<Iter>& ctx, const Ast<Iter>* ast){
	ctx.matches.pop_back();
	ctx.expecteds.emplace_back(ast);
}

template<class Iter, class Matcher>
bool do_match(Ctx<Iter>& ctx, const Ast<Iter>* ast, Matcher&& m){
	if(ctx.skipper) ctx.iter = ctx.skipper(ctx.iter, ctx.end);
	Iter begin = ctx.iter;
	auto [success, inc] = m(ctx);
	if(success){
		ctx.iter += inc;
		ctx.expecteds.clear();
		ctx.match_queue.emplace_back(ast, begin, ctx.iter);
		return true;
		
	} else {
		ctx.expecteds.push_back(ast);
		return false;
	}
}

}
}
}
