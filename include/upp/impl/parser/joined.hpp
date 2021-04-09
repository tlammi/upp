#pragma once

#include "upp/impl/parser/matcher.hpp"
#include "upp/impl/parser/ast.hpp"
#include "upp/impl/parser/detail/type_traits.hpp"

namespace upp{
namespace parser{

template<class Iter, class Lhs, class Rhs>
class Joined: public Matcher<Iter>{
public:
	Joined(Lhs l, Rhs r): l_{l}, r_{r}{}

	Result<Iter> match(Iter begin, Iter end, Iter(*skipper)(Iter, Iter)) const final {
		auto res = l_.match(begin, end, skipper);
		if(!res) return {begin, false};
		return r_.match(res.iter, end, skipper);
	}

private:
	Lhs l_;
	Rhs r_;
};

template<class Lhs, class Rhs, class = std::enable_if<detail::is_ast_v<Lhs> && detail::is_ast_v<Rhs>>>
Ast<detail::iter_t<Lhs>, Joined<detail::iter_t<Lhs>, Lhs, Rhs>, void> operator,(Lhs&& l, Rhs&& r){
	        static_assert(std::is_same_v<detail::iter_t<Lhs>, detail::iter_t<Rhs>>);
		return {Joined<detail::iter_t<Lhs>, Lhs, Rhs>(std::forward<Lhs>(l), std::forward<Rhs>(r))};
}

}
}
