#pragma once

#include <memory>

#include "upp/impl/parser/detail/cbholder.hpp"
#include "upp/impl/parser/detail/ctx.hpp"

namespace upp{
namespace parser{
namespace detail{


template<class T, class Iter>
bool match(T&& t, Ctx<Iter>& ctx){
	return t.match(ctx);
}


template<class T, class Iter>
void invoke(T&& t, Iter begin, Iter end){
	return t.invoke(begin, end);
}

}

/**
 * Interface for ASTs
 *
 * Implemented so \ref DynAst can store any Ast and perform a
 * recursive match.
 */
template<class Iter>
class Ast{
	template<class T, class Iter2>
	friend bool detail::match(T&&, detail::Ctx<Iter2>&);
	template<class T, class Iter2>
	friend void detail::invoke(T&&, Iter2, Iter2);
public:

	virtual ~Ast(){}

	/// Name of the AST. Used for error printing
	virtual std::string_view name() const noexcept {
		return "<nameless ast>";
	}

private:

	bool match(detail::Ctx<Iter>& ctx) const noexcept {
		return match_(ctx);
	}

	void invoke(Iter begin, Iter end) const {
		invoke_(begin, end);
	}

	/**
	 * AST-specific matching
	 *
	 * Method that performs the matching. Mainly checks the current state of the detail::Ctx
	 * and moves iterator if required.
	 *
	 * \param ctx Context for the match. A structure containing runtime data of the match.
	 * \return true, if a successfull match, false otherwise
	 */
	virtual bool match_(detail::Ctx<Iter>& ctx) const noexcept = 0;

	/**
	 * Used for dispatching match callbacks
	 *
	 * Parser implementation calls this method after matching to invoke callbacks
	 * registered to ASTs. This is placed in a separate method rather than integrated
	 * to \ref match_() so the callbacks are invoked in more intuitive order
	 *
	 * \param begin Iterator pointing to beginning of the sequence matched by the AST
	 * \param end Iterator pointing to end of the sequence matched by the AST
	 */
	virtual void invoke_(Iter begin, Iter end) const = 0;
};

/**
 * Dynamic AST
 *
 * Unlike other AST implementations, this stores a generic \ref Ast instance.
 * This class has two use cases: either to perform recursive matching or to
 * store an \ref Ast in a class, where template arguments cannot be automatically
 * deduced.
 *
 * \tparam Iter Matching iterator
 * \tparam OnMatch callback invoked on match
 */
template<class Iter, class OnMatch=std::nullptr_t>
class DynAst: public Ast<Iter>{
public:

	template<class OnMatch2>
	DynAst(DynAst<Iter, OnMatch2>&& other, OnMatch on_match):
		ast_{std::move(other.ast_)}, on_match_{std::forward<OnMatch>(on_match)}{}

	template<class OnMatch2>
	DynAst(const DynAst<Iter, OnMatch2>& other, OnMatch on_match):
		ast_{std::move(other.ast_)}, on_match_{std::forward<OnMatch>(on_match)}{}

	/// Create empty DynAst with callback only
	DynAst(OnMatch&& on_match): on_match_{on_match}{}

	/**
	 * Create an operational DynAst
	 *
	 * \param ptr Pointer to Ast. Ownership is claimed by the object
	 * \param on_match Callback invoked if this AST matches
	 */
	DynAst(Ast<Iter>* ptr, OnMatch&& on_match): ast_{ptr}, on_match_{on_match}{}

	/// Construct DynAst without the matching callback
	DynAst(Ast<Iter>* ptr): ast_{ptr}{}

	/**
	 * Assign an Ast to this object
	 *
	 * \param other May be any type derived from \ref Ast
	 */
	template<class Other, class = std::enable_if_t<!std::is_same_v<std::decay_t<Other>, DynAst>>>
	DynAst& operator=(Other&& other){
		ast_ = std::make_unique<std::decay_t<Other>>(std::move(other));
		return *this;
	}
private:

	bool match_(detail::Ctx<Iter>& ctx) const noexcept final {
		if(ast_){
			Iter* end = detail::prepare_match(ctx, this, on_match_);
			bool res = detail::match(*ast_, ctx);
			if(res)
				detail::register_match(ctx, end, 0);
			else
				detail::register_miss(ctx, end);
			return res;
		}
		return false;
	}

	void invoke_(Iter begin, Iter end) const final {
		on_match_(begin, end);
	}

	std::unique_ptr<Ast<Iter>> ast_{nullptr};
	detail::CbHolder<OnMatch> on_match_{nullptr};
};

}
}

