#pragma once

#include "upp/impl/parser/ast.hpp"
#include "upp/impl/parser/matcher.hpp"

namespace upp{
namespace parser{


template<class Iter, class AstKind>
class KleenePlus: public Matcher<Iter>{
public:

	KleenePlus(AstKind a): a_{a}{}

	bool match(Ctx<Iter>& ctx) const final {
		auto res = a_.match(ctx);
		if(!res) return res;

		while(true){
			res = a_.match(ctx);
			if(!res) return true;
		}
	}

private:
	AstKind a_;
};

template<class A, class = std::enable_if<detail::is_ast_v<A>>>
Ast<detail::iter_t<A>, KleenePlus<detail::iter_t<A>, A>, void> operator+(A&& a){
	return {KleenePlus<detail::iter_t<A>, A>(std::forward<A>(a))};
}


}
}
