#pragma once

#include "upp/impl/parser/ast.hpp"
#include "upp/impl/parser/matcher.hpp"

namespace upp{
namespace parser{


template<class Iter, class AstKind>
class KleeneStar: public Matcher<Iter>{
public:
	using iterator = Iter;

	KleeneStar(AstKind a): a_{a}{}

	Result<Iter> match(Iter iter, Iter end) const final {
		while(true){
			auto res = a_.match(iter, end);
			if(res)
				iter = res.iter;
			else
				return {iter, true};
		}
	}

private:
	AstKind a_;
};


template<class Iter, class M, class OnMatch>
Ast<Iter, KleeneStar<Iter, Ast<Iter, M, OnMatch>&>, void> operator*(Ast<Iter, M, OnMatch>& ast){
	return {KleeneStar<Iter, Ast<Iter, M, OnMatch>&>(ast)};
}

template<class Iter, class M, class OnMatch>
Ast<Iter, KleeneStar<Iter, Ast<Iter, M, OnMatch>>, void> operator*(Ast<Iter, M, OnMatch>&& ast){
	return {KleeneStar<Iter, Ast<Iter, M, OnMatch>>(std::move(ast))};
}

}
}
