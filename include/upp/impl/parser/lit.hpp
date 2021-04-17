#pragma once

#include "upp/impl/parser/ast.hpp"

namespace upp{
namespace parser{


template<class Iter, class OnMatch=std::nullptr_t>
class LiteralChar: public Ast<Iter>{
public:
	LiteralChar(char c): c_{c}{}
	LiteralChar(char c, OnMatch&& on_match): c_{c}, cb_{std::move(on_match)}{}


	std::string_view name() const noexcept final {
		return std::string_view(&c_, 1);
	}

private:
	bool match_(detail::Ctx<Iter>& ctx) const noexcept final {
		Iter* end = detail::prepare_match(ctx, this, cb_);
		if(ctx.iter != ctx.end && *ctx.iter == c_){
			detail::register_match_leaf(ctx, end, 1);
			return true;
		}
		detail::register_miss_leaf(ctx, this, end);
		return false;
	}

	void invoke_(Iter begin, Iter end) const final {
		cb_(begin, end);
	}

	char c_;
	detail::CbHolder<OnMatch> cb_{};
};

template<class Iter, class OnMatch=std::nullptr_t>
class LiteralStr: public Ast<Iter>{
public:
	LiteralStr(std::string_view str): str_{str}{}

	std::string_view name() const noexcept final {
		return str_;
	}

private:
	bool match_(detail::Ctx<Iter>& ctx) const noexcept final {
		auto is_equal =  [&](){
			if(ctx.end - ctx.iter < str_.end() - str_.begin())
				return false;
			return std::equal(str_.begin(), str_.end(), ctx.iter);
		};


		Iter* end = detail::prepare_match(ctx, this, on_match_);

		if(is_equal()){
			detail::register_match_leaf(ctx, end, str_.size());
			return true;
		}
		detail::register_miss_leaf(ctx, this, end);
		return false;
	}

	void invoke_(Iter begin, Iter end) const final {
		on_match_(begin, end);
	}
	std::string str_;
	detail::CbHolder<OnMatch> on_match_{};
};


}
}
