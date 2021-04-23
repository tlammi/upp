#pragma once

#include "upp/impl/parser/ast.hpp"

namespace upp{
namespace parser{


/**
 * Ast matching a character literal
 */
template<class Iter, class OnMatch=std::nullptr_t>
class LiteralChar: public Ast<Iter>{
	template<class, class>
	friend class LiteralChar;
public:
	LiteralChar(char c): c_{c}{}
	LiteralChar(char c, OnMatch&& on_match): c_{c}, cb_{std::move(on_match)}{}

	template<class OnMatch2>
	LiteralChar(LiteralChar<Iter, OnMatch2>&& other, OnMatch&& on_match):
		c_{other.c_}, cb_{std::move(on_match)}{}

	template<class OnMatch2>
	LiteralChar(const LiteralChar<Iter, OnMatch2>& other, OnMatch&& on_match):
		c_{other.c_}, cb_{std::move(on_match)}{}


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

/**
 * Ast matching a string sequence
 */
template<class Iter, class OnMatch=std::nullptr_t>
class LiteralStr: public Ast<Iter>{

	template<class, class>
	friend class LiteralStr;

public:
	LiteralStr(std::string_view str): str_{str}{}

	LiteralStr(std::string_view str, OnMatch&& on_match): str_{str}, on_match_{std::move(on_match)}{}

	template<class OnMatch2>
	LiteralStr(LiteralStr<Iter, OnMatch2>&& other, OnMatch&& on_match):
		str_{std::move(other.str_)}, on_match_{std::move(on_match)}{}

	template<class OnMatch2>
	LiteralStr(const LiteralStr<Iter, OnMatch2>& other, OnMatch&& on_match):
		str_{other.str_}, on_match_{std::move(on_match)}{}


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
