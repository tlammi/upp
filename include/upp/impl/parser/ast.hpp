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


template<class Iter>
class Ast{
	template<class T, class Iter2>
	friend bool detail::match(T&&, detail::Ctx<Iter2>&);
	template<class T, class Iter2>
	friend void detail::invoke(T&&, Iter2, Iter2);
public:

private:
	bool match(detail::Ctx<Iter>& ctx) const noexcept {
		return match_(ctx);
	}

	void invoke(Iter begin, Iter end) const {
		invoke_(begin, end);
	}

	virtual bool match_(detail::Ctx<Iter>& ctx) const noexcept = 0;
	virtual void invoke_(Iter begin, Iter end) const = 0;
};


template<class Iter, class OnMatch=std::nullptr_t>
class DynAst: public Ast<Iter>{
public:
	DynAst(Ast<Iter>* ptr, OnMatch&& on_match): ast_{ptr}, on_match_{on_match}{}
	DynAst(Ast<Iter>* ptr): ast_{ptr}{}

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
				detail::register_miss(ctx, this, end);
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

