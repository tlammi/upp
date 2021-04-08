#pragma once

#include "upp/impl/parser/ast.hpp"
#include "upp/impl/parser/matcher.hpp"

namespace upp{
namespace parser{


template<class Iter>
class LiteralChar: public Matcher<Iter>{
public:
	using iterator = Iter;

	LiteralChar(char c): c_{c}{}

	Result<Iter> match(Iter iter, Iter end) const final {
		if(iter != end && *iter == c_)
			return {iter+1, true};
		return {iter, false};
	}

private:
	char c_;
};

template<class Iter>
class LiteralStr: public Matcher<Iter>{
public:
	using iterator = Iter;

	LiteralStr(std::string_view str): str_{str}{}

	Result<Iter> match(Iter begin, Iter end) const final {
		if(end - begin < str_.end() - str_.begin())
			return {begin, false};

		if(std::equal(str_.begin(), str_.end(), begin))
			return {begin+str_.size(), true};
		return {begin, false};
	}

private:
	std::string str_;
};


}
}
