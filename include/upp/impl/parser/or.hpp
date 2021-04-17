#pragma once

#include "upp/impl/parser/ast.hpp"
#include "upp/impl/parser/detail/type_traits.hpp"

namespace upp{
namespace parser{


template<class Iter, class Lhs, class Rhs, class OnMatch=std::nullptr_t>
class Or: public Ast<Iter>{
public:

	Or(Lhs l, Rhs r, OnMatch&& on_match): l_{l}, r_{r}, cb_{std::move(on_match)}{}
	Or(Lhs l, Rhs r): l_{l}, r_{r}{}

private:
	bool match_(detail::Ctx<Iter>& ctx) const final {
		Iter* end = detail::prepare_match(ctx, this, cb_);
		bool res = detail::match(l_, ctx);
		if(!res) res = detail::match(r_, ctx);
		if(res)
			detail::register_match(ctx, end, 0);
		else
			detail::register_miss(ctx, this, end);
		return res;
	}

	void invoke_(Iter begin, Iter end) const final {
		cb_(begin, end);
	}

	Lhs l_;
	Rhs r_;

	detail::CbHolder<OnMatch> cb_{};
};

template<class Lhs, class Rhs, class = std::enable_if<detail::is_ast_v<Lhs> && detail::is_ast_v<Rhs>>>
Or<detail::iter_t<Lhs>, Lhs, Rhs> operator|(Lhs&& l, Rhs&& r){
	static_assert(std::is_same_v<detail::iter_t<Lhs>, detail::iter_t<Rhs>>);
	return {std::forward<Lhs>(l), std::forward<Rhs>(r)};
}

}
}
