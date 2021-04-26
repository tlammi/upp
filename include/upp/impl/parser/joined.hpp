#pragma once

#include "upp/impl/parser/ast.hpp"
#include "upp/impl/parser/detail/type_traits.hpp"

#include <string>

namespace upp{
namespace parser{

/**
 * An AST representing a pair of sub asts in sequential order
 *
 * A wrapper class implementing Ast and wrapping two sub Asts: left and right.
 * The object is used as a return value of comma operator. (MyAstA<Iter>(), MyAstB<Iter>())
 * would return a Joined<Iter, MyAstA, MyAstB>.
 * 
 */
template<class Iter, class Lhs, class Rhs, class OnMatch=std::nullptr_t>
class Joined: public Ast<Iter>{
	template<class, class, class, class>
	friend class Joined;
public:
	Joined(Lhs l, Rhs r, OnMatch&& on_match): l_{std::forward<Lhs>(l)}, r_{std::forward<Rhs>(r)}, on_match_{std::move(on_match)}{}
	Joined(Lhs l, Rhs r): l_{std::forward<Lhs>(l)}, r_{std::forward<Rhs>(r)}{}

	template<class OnMatch2>
	Joined(Joined<Iter, Lhs, Rhs, OnMatch2>&& other, OnMatch&& on_match):
		l_{std::forward<Lhs>(other.l_)}, r_{std::forward<Rhs>(other.r_)}, on_match_{on_match}{}

	template<class OnMatch2>
	Joined(const Joined<Iter, Lhs, Rhs, OnMatch2>& other, OnMatch&& on_match):
		l_{other.l_}, r_{other.r_}, on_match_{on_match}{}

private:

	bool match_(detail::Ctx<Iter>& ctx) const noexcept final {
		Iter begin = ctx.iter;
		Iter* end = detail::prepare_match(ctx, this, on_match_);
		auto res = detail::match(l_, ctx);
		if(!res){
			detail::register_miss(ctx, end);
			return false;
		}
		res = detail::match(r_, ctx);
		if(!res){
			detail::register_miss(ctx, end);
			ctx.iter = begin;
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

/**
 * Comma operator for all Ast types
 *
 * Wraps the two objects in a parent object. Rvalues are moved into the wrapper object
 * and lvalues are stored as references.
 *
 * \param l Ast that should come first in the sequence
 * \param r Ast that should come second in the sequence
 * \return Ast wrapping the two operands
 */
template<class Lhs, class Rhs, class = std::enable_if_t<detail::is_ast_v<Lhs> && detail::is_ast_v<Rhs>>>
Joined<detail::iter_t<Lhs>, Lhs, Rhs> operator,(Lhs&& l, Rhs&& r){
	        static_assert(std::is_same_v<detail::iter_t<Lhs>, detail::iter_t<Rhs>>);
		return {std::forward<Lhs>(l), std::forward<Rhs>(r)};
}

}
}
