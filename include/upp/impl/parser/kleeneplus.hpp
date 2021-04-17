#pragma once

#include "upp/impl/parser/ast.hpp"
#include "upp/impl/parser/detail/cbholder.hpp"

namespace upp{
namespace parser{


template<class Iter, class SubAst, class OnMatch=std::nullptr_t>
class KleenePlus: public Ast<Iter>{
public:

	KleenePlus(SubAst a): a_{a}{}

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

template<class A, class = std::enable_if_t<detail::is_ast_v<A>>>
KleenePlus<detail::iter_t<A>, A> operator+(A&& a){
	return {std::forward<A>(a)};
}


}
}
