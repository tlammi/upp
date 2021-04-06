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

	Result<Iter> match(Iter begin, Iter end) const final {	
		auto reiter = std::regex_iterator<Iter>(begin, end, re_, std::regex_constants::match_continuous);
		if(reiter != std::regex_iterator<Iter>())
			return {begin + reiter->str().size(), true};
		return {begin, false};
	}

private:

	std::regex re_;
};

}
}
