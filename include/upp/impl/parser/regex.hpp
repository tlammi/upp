#pragma once

#include <regex>
#include <string_view>

#include "upp/impl/parser/matcher.hpp"
#include "upp/impl/parser/result.hpp"

namespace upp{
namespace parser{

template<class Iter>
class Regex: public Matcher<Iter> {
public:
	Regex(std::string_view re): re_{re.begin(), re.end()}{}

	bool match(Ctx<Iter>& ctx) const final {	
		auto reiter = std::regex_iterator<Iter>(ctx.iter, ctx.end, re_, std::regex_constants::match_continuous);
		if(reiter != std::regex_iterator<Iter>()){
			ctx.iter += reiter->str().size();
			ctx.misses.clear();
			return true;
		}
		ctx.misses.push_back("<regex>");
		return false;
	}

private:

	std::regex re_;
};

}
}
