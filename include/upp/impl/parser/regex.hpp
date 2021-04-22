#pragma once

#include <regex>
#include <string_view>

#include "upp/impl/parser/ast.hpp"
#include "upp/impl/parser/detail/cbholder.hpp"

namespace upp{
namespace parser{

/**
 * Ast matching a reqular expression
 */
template<class Iter, class OnMatch=std::nullptr_t>
class Regex: public Ast<Iter> {

	template<class, class>
	friend class Regex;
public:
	Regex(std::string_view re, OnMatch&& on_match): re_{re.begin(), re.end()}, cb_{std::move(on_match)}{}
	Regex(std::string_view re): re_{re.begin(), re.end()}{}

	template<class OnMatch2>
	Regex(Regex<Iter, OnMatch2>&& other, OnMatch&& on_match):
		re_{std::move(other.re_)}, cb_{std::forward<OnMatch>(on_match)}{}

	template<class OnMatch2>
	Regex(const Regex<Iter, OnMatch2>& other, OnMatch&& on_match):
		re_{other.re_}, cb_{std::forward<OnMatch>(on_match)}{}

private:
	bool match_(detail::Ctx<Iter>& ctx) const noexcept final {
		Iter* end = detail::prepare_match(ctx, this, cb_);
		auto reiter = std::regex_iterator<Iter>(ctx.iter, ctx.end, re_, std::regex_constants::match_continuous);
		if(reiter != std::regex_iterator<Iter>()){
			detail::register_match_leaf(ctx, end, reiter->str().size());
			return true;
		}
		detail::register_miss_leaf(ctx, this, end);
		return false;
	}

	void invoke_(Iter begin, Iter end) const final {
		cb_(begin, end);
	}

	std::regex re_;
	detail::CbHolder<OnMatch> cb_{};
};

}
}
