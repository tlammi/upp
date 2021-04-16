#pragma once

#include "upp/impl/parser/ast.hpp"
#include "upp/impl/parser/detail/cbholder.hpp"

namespace upp{
namespace parser{


template<class Iter, class SubAst, class OnMatch=std::nullptr_t>
class Optional: public Ast<Iter>{
public:

	Optional(SubAst a): a_{a}{}

private:
	bool match_(detail::Ctx<Iter>& ctx) const final {
		auto& end = detail::prepare_match(ctx, this);
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

template<class A, class = std::enable_if_t<detail::is_ast_v<A>>>
Optional<detail::iter_t<A>, A> operator-(A&& a){
	return {std::forward<A>(a)};
}


}
}
