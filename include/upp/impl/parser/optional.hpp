#pragma once

#include "upp/impl/parser/ast.hpp"
#include "upp/impl/parser/matcher.hpp"

namespace upp{
namespace parser{


template<class Iter, class AstKind>
class Optional: public Matcher<Iter>{
public:
	using iterator = Iter;

	Optional(AstKind a): a_{a}{}

	Result<Iter> match(Iter begin, Iter end) const final {
		auto res = a_.match(begin, end);
		return {res.iter, true};
	}

private:
	AstKind a_;
};


template<class Iter, class M, class OnMatch>
Ast<Iter, Optional<Iter, Ast<Iter, M, OnMatch>&>, void> operator-(Ast<Iter, M, OnMatch>& ast){
	return {Optional<Iter, Ast<Iter, M, OnMatch>&>(ast)};
}

template<class Iter, class M, class OnMatch>
Ast<Iter, Optional<Iter, Ast<Iter, M, OnMatch>>, void> operator-(Ast<Iter, M, OnMatch>&& ast){
	return {Optional<Iter, Ast<Iter, M, OnMatch>>(std::move(ast))};
}

}
}
