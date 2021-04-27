#pragma once

#include "upp/impl/parser/ast.hpp"
#include "upp/impl/parser/detail/cbholder.hpp"

namespace upp{
namespace parser{


/**
 * Ast specialization matching an Ast [1, inf) times
 * 
 * This class wraps the sub Ast and only calls that types
 * match() method as many times as possible. The match
 * fails if 0 matches are found. Otherwise succeeds.
 */
template<class Iter, class SubAst, class OnMatch=std::nullptr_t>
class KleenePlus: public Ast<Iter>{
	template<class, class, class>
	friend class KleenePlus;

public:

	KleenePlus(SubAst a): a_{std::forward<SubAst>(a)}{}

	template<class OnMatch2>
	KleenePlus(KleenePlus<Iter, SubAst, OnMatch2>&& other, OnMatch&& on_match):
		a_{std::forward<SubAst>(other.a_)}, on_match_{std::move(on_match)}{}

	template<class OnMatch2>
	KleenePlus(const KleenePlus<Iter, SubAst, OnMatch2>& other, OnMatch&& on_match):
		a_{other.a_}, on_match_{std::move(on_match)}{}

	KleenePlus(const KleenePlus&) = default;
	KleenePlus(KleenePlus&&) = default;

	KleenePlus& operator=(const KleenePlus&) = default;
	KleenePlus& operator=(KleenePlus&&) = default;

private:
	bool match_(detail::Ctx<Iter>& ctx) const noexcept final {
		Iter* end = detail::prepare_match(ctx, this, on_match_);
		auto res = detail::match(a_, ctx);
		if(!res){
			detail::register_miss(ctx, end);
			return res;
		}
		while(true){
			res = detail::match(a_, ctx);
			if(!res) break;
		}
		detail::register_match(ctx, end, 0);
		return true;
	}

	void invoke_(Iter begin, Iter end) const final {
		on_match_(begin, end);
	}

	SubAst a_;
	detail::CbHolder<OnMatch> on_match_{};
};

/// Shorthand for creating a kleeneplus Ast
template<class A, class = std::enable_if_t<detail::is_ast_v<A>>>
KleenePlus<detail::iter_t<A>, A> operator+(A&& a){
	return {std::forward<A>(a)};
}


}
}
