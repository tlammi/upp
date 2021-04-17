#pragma once

#include "upp/impl/parser/ast.hpp"
#include "upp/impl/parser/detail/type_traits.hpp"

namespace upp{
namespace parser{

template<class Iter, class Lhs, class Rhs, class OnMatch=std::nullptr_t>
class Joined: public Ast<Iter>{
	template<class, class, class, class>
	friend class Joined;
public:
	Joined(Lhs l, Rhs r, OnMatch&& on_match): l_{l}, r_{r}, on_match_{std::move(on_match)}{}
	Joined(Lhs l, Rhs r): l_{l}, r_{r}{}

	template<class OnMatch2>
	Joined(Joined<Iter, Lhs, Rhs, OnMatch2>&& other, OnMatch&& on_match):
		l_{other.l_}, r_{other.r_}, on_match_{on_match}{}

private:

	bool match_(detail::Ctx<Iter>& ctx) const noexcept final {
		Iter* end = detail::prepare_match(ctx, this, on_match_);
		auto res = detail::match(l_, ctx);
		if(!res){
			detail::register_miss(ctx, end);
			return false;
		}
		res = detail::match(r_, ctx);
		if(!res){
			detail::register_miss(ctx, end);
			return false;
		}
		detail::register_match(ctx, end, 0);
		return true;
	}


	void invoke_(Iter begin, Iter end) const final {
		on_match_(begin, end);
	}

	Lhs l_;
	Rhs r_;
	detail::CbHolder<OnMatch> on_match_{};
};

template<class Lhs, class Rhs, class = std::enable_if<detail::is_ast_v<Lhs> && detail::is_ast_v<Rhs>>>
Joined<detail::iter_t<Lhs>, Lhs, Rhs> operator,(Lhs&& l, Rhs&& r){
	        static_assert(std::is_same_v<detail::iter_t<Lhs>, detail::iter_t<Rhs>>);
		return {std::forward<Lhs>(l), std::forward<Rhs>(r)};
}

}
}
