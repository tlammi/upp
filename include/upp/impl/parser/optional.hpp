#pragma once

#include "upp/impl/parser/ast.hpp"
#include "upp/impl/parser/detail/cbholder.hpp"

namespace upp{
namespace parser{


/**
 * Ast matching the sub ast [0, 1] times
 *
 * Calls \ref match() of the sub matcher and converts a failing
 * match to a success without moving the iterator.
 */
template<class Iter, class SubAst, class OnMatch=std::nullptr_t>
class Optional: public Ast<Iter>{
	template<class, class, class>
	friend class Optional;
public:

	Optional(SubAst a): a_{std::forward<SubAst>(a)}{}

	template<class OnMatch2>
	Optional(Optional<Iter, SubAst, OnMatch2>&& other, OnMatch&& on_match):
		a_{std::forward<SubAst>(other.a_)}, on_match_{std::move(on_match)}{}

	template<class OnMatch2>
	Optional(const Optional<Iter, SubAst, OnMatch2>& other, OnMatch&& on_match):
		a_{other.a_}, on_match_{std::move(on_match)}{}

	Optional(const Optional&) = default;
	Optional(Optional&&) = default;

	Optional& operator=(const Optional&) = default;
	Optional& operator=(Optional&&) = default;

private:
	bool match_(detail::Ctx<Iter>& ctx) const noexcept final {
		auto* end = detail::prepare_match(ctx, this, on_match_);
		(void) detail::match(a_, ctx);
		detail::register_match(ctx, end, 0);
		return true;
	}


	void invoke_(Iter begin, Iter end) const final {
		on_match_(begin, end);
	}

	SubAst a_;
	detail::CbHolder<OnMatch> on_match_{};
};

/// Shorthand for creating an Optional in grammar
template<class A, class = std::enable_if_t<detail::is_ast_v<A>>>
Optional<detail::iter_t<A>, A> operator-(A&& a){
	return {std::forward<A>(a)};
}


}
}
