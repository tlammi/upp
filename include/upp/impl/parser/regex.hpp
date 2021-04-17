#pragma once

#include <regex>
#include <string_view>

#include "upp/impl/parser/ast.hpp"
#include "upp/impl/parser/detail/cbholder.hpp"

namespace upp{
namespace parser{

template<class Iter, class OnMatch=std::nullptr_t>
class Regex: public Ast<Iter> {
public:
	Regex(std::string_view re, OnMatch&& on_match): re_{re.begin(), re.end()}, cb_{std::move(on_match)}{}
	Regex(std::string_view re): re_{re.begin(), re.end()}{}

private:
	bool match_(detail::Ctx<Iter>& ctx) const noexcept final {
		Iter* end = detail::prepare_match(ctx, this, cb_);
		auto reiter = std::regex_iterator<Iter>(ctx.iter, ctx.end, re_, std::regex_constants::match_continuous);
		if(reiter != std::regex_iterator<Iter>()){
			detail::register_match(ctx, end, reiter->str().size());
			return true;
		}
		detail::register_miss(ctx, this, end);
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
