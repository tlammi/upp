#pragma once

#include "upp/impl/parser/matcher.hpp"
#include "upp/impl/parser/ast.hpp"
#include "upp/impl/parser/type_traits.hpp"

namespace upp{
namespace parser{

template<class Iter, class Lhs, class Rhs>
class Joined: public Matcher<Iter>{
public:
	Joined(Lhs l, Rhs r): l_{l}, r_{r}{}

	Result<Iter> match(Iter begin, Iter end) const final {
		auto res = l_.match(begin, end);
		if(!res) return {begin, false};
		return r_.match(res.iter, end);
	}

private:
	Lhs l_;
	Rhs r_;
};

template<class Iter, class M0, class M1, class OnMatch0, class OnMatch1>
Ast<Iter, Joined<Iter, Ast<Iter, M0, OnMatch0>&, Ast<Iter, M1, OnMatch1>&>, void> operator,(Ast<Iter, M0, OnMatch0>& lhs, Ast<Iter, M1, OnMatch1>& rhs){
	return {Joined<Iter, Ast<Iter, M0, OnMatch0>&, Ast<Iter, M1, OnMatch1>&>{lhs, rhs}};
}
}
}
