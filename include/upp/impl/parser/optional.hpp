#pragma once

#include "upp/impl/parser/ast.hpp"
#include "upp/impl/parser/matcher.hpp"

namespace upp{
namespace parser{


template<class Iter, class AstKind>
class Optional: public Matcher<Iter>{
public:

	Optional(AstKind a): a_{a}{}

	bool match(Ctx<Iter>& ctx) const final {
		(void) a_.match(ctx);
		return true;
	}

private:
	AstKind a_;
};

template<class A, class = std::enable_if<detail::is_ast_v<A>>>
Ast<detail::iter_t<A>, Optional<detail::iter_t<A>, A>, void> operator-(A&& a){
	return {Optional<detail::iter_t<A>, A>(std::forward<A>(a))};
}


}
}
