#pragma once

#include "upp/impl/parser/ast.hpp"
#include "upp/impl/parser/matcher.hpp"
#include "upp/impl/parser/detail/type_traits.hpp"

namespace upp{
namespace parser{


template<class Iter, class Lhs, class Rhs>
class Or: public Matcher<Iter>{
public:

	Or(Lhs l, Rhs r): l_{l}, r_{r}{}

	bool match(Ctx<Iter>& ctx) const final {
		auto res = l_.match(ctx);
		if(res) return res;
		return r_.match(ctx);
	}

private:
	Lhs l_;
	Rhs r_;
};

template<class Lhs, class Rhs, class = std::enable_if<detail::is_ast_v<Lhs> && detail::is_ast_v<Rhs>>>
Ast<detail::iter_t<Lhs>, Or<detail::iter_t<Lhs>, Lhs, Rhs>, void> operator|(Lhs&& l, Rhs&& r){
	static_assert(std::is_same_v<detail::iter_t<Lhs>, detail::iter_t<Rhs>>);
	return {Or<detail::iter_t<Lhs>, Lhs, Rhs>(std::forward<Lhs>(l), std::forward<Rhs>(r))};
}

}
}
