#pragma once

#include "upp/impl/parser/result.hpp"

namespace upp{
namespace parser{


auto whitespaces(auto iter, auto end){
	while(iter != end && std::isspace(*iter)) ++iter;
	return iter;
}


auto spaces(auto iter, auto end){
	while(iter != end && *iter == ' ') ++iter;
	return iter;
}

template<class Iter, class A>
Result<Iter> parse(Iter begin, Iter end, A&& ast, Iter(*skipper)(Iter, Iter)=nullptr){
	detail::Ctx<Iter> ctx{begin, end, skipper};
	bool res = detail::match(ast, ctx);
	for(const auto [ptr, mbegin, mend]: ctx.matches){
		detail::invoke(*ptr, mbegin, mend);

	}
	return Result<Iter>{ctx.iter, res, ctx.expecteds};
}

}
}
