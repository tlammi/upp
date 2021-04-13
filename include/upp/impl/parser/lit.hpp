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

	bool match(Ctx<Iter>& ctx) const final {
		if(ctx.iter != ctx.end && *ctx.iter == c_){
			++ctx.iter;
			ctx.misses.clear();
			return true;
		}
		ctx.misses.emplace_back(&c_, 1);
		return false;
	}

private:
	char c_;
};

template<class Iter>
class LiteralStr: public Matcher<Iter>{
public:
	using iterator = Iter;

	LiteralStr(std::string_view str): str_{str}{}

	bool match(Ctx<Iter>& ctx) const final {
		auto is_equal =  [&](){
			if(ctx.end - ctx.iter < str_.end() - str_.begin())
				return false;
			return std::equal(str_.begin(), str_.end(), ctx.iter);
		};

		if(is_equal()){
			ctx.iter += str_.size();
			ctx.misses.clear();
			return true;
		}
		
		ctx.misses.emplace_back(str_);
		return false;
	}

private:
	std::string str_;
};


}
}
