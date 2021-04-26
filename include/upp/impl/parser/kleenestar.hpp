#pragma once

#include "upp/impl/parser/ast.hpp"
#include "upp/impl/parser/detail/cbholder.hpp"

namespace upp{
namespace parser{

/**
 * Ast matching [0, inf) instances of the sub Ast
 *
 * Invokes sub Ast's \ref match_() method 0 to inf times, as many times as possible.
 */
template<class Iter, class SubAst, class OnMatch=std::nullptr_t>
class KleeneStar: public Ast<Iter>{

	template<class, class, class>
	friend class KleeneStar;

public:

	KleeneStar(SubAst a): a_{std::forward<SubAst>(a)}{}
	KleeneStar(SubAst a, OnMatch&& on_match):
		a_{std::forward<Ast>(a)}, on_match_{std::move(on_match)}{}

	template<class OnMatch2>
	KleeneStar(KleeneStar<Iter, SubAst, OnMatch2>&& other, OnMatch&& on_match):
		a_{std::forward<SubAst>(other.a_)}, on_match_{std::move(on_match)}{}

	template<class OnMatch2>
	KleeneStar(const KleeneStar<Iter, SubAst, OnMatch2>& other, OnMatch&& on_match):
		a_{other.a_}, on_match_{std::move(on_match)}{}

private:
	bool match_(detail::Ctx<Iter>& ctx) const noexcept final {
		Iter* end = detail::prepare_match(ctx, this, on_match_);
		while(true){
			auto res = detail::match(a_, ctx);
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

/// Shorthand for creating kleene star Ast in grammar
template<class A, class = std::enable_if_t<detail::is_ast_v<A>>>
KleeneStar<detail::iter_t<A>, A> operator*(A&& a){
		return {std::forward<A>(a)};
}

}
}
