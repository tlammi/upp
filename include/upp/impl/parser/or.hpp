#pragma once

#include "upp/impl/parser/ast.hpp"
#include "upp/impl/parser/matcher.hpp"

namespace upp{
namespace parser{


template<class Iter, class Lhs, class Rhs>
class Or: public Matcher<Iter>{
public:
	using iterator = Iter;

	Or(Lhs l, Rhs r): l_{l}, r_{r}{}

	Result<Iter> match(Iter begin, Iter end) const final {
		auto res = l_.match(begin, end);
		if(res) return res;
		return r_.match(begin, end);
	}

private:
	Lhs l_;
	Rhs r_;
};



template<class Iter, class M0, class M1, class OnMatch0, class OnMatch1>
Ast<Iter, Or<Iter, Ast<Iter, M0, OnMatch0>&, Ast<Iter, M1, OnMatch1>&>, void> operator|(Ast<Iter, M0, OnMatch0>& lhs, Ast<Iter, M1, OnMatch1>& rhs){
	        return {Or<Iter, Ast<Iter, M0, OnMatch0>&, Ast<Iter, M1, OnMatch1>&>{lhs, rhs}};
}

template<class Iter, class M0, class M1, class OnMatch0, class OnMatch1>
Ast<Iter, Or<Iter, Ast<Iter, M0, OnMatch0>&, Ast<Iter, M1, OnMatch1>>, void> operator|(Ast<Iter, M0, OnMatch0>& lhs, Ast<Iter, M1, OnMatch1>&& rhs){
	        return {Or<Iter, Ast<Iter, M0, OnMatch0>&, Ast<Iter, M1, OnMatch1>>{lhs, std::move(rhs)}};
}

template<class Iter, class M0, class M1, class OnMatch0, class OnMatch1>
Ast<Iter, Or<Iter, Ast<Iter, M0, OnMatch0>, Ast<Iter, M1, OnMatch1>&>, void> operator|(Ast<Iter, M0, OnMatch0>&& lhs, Ast<Iter, M1, OnMatch1>& rhs){
	        return {Or<Iter, Ast<Iter, M0, OnMatch0>, Ast<Iter, M1, OnMatch1>&>{std::move(lhs), rhs}};
}

template<class Iter, class M0, class M1, class OnMatch0, class OnMatch1>
Ast<Iter, Or<Iter, Ast<Iter, M0, OnMatch0>, Ast<Iter, M1, OnMatch1>>, void> operator|(Ast<Iter, M0, OnMatch0>&& lhs, Ast<Iter, M1, OnMatch1>&& rhs){
	        return {Or<Iter, Ast<Iter, M0, OnMatch0>, Ast<Iter, M1, OnMatch1>>{std::move(lhs), std::move(rhs)}};
}

}
}
