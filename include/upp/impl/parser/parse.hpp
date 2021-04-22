#pragma once

#include "upp/impl/parser/result.hpp"

namespace upp{
namespace parser{

namespace detail{

struct IterCoord{
	size_t line{1};
	size_t column{0};
};

template<class Iter>
IterCoord coord(Iter iter, const Iter pos, const Iter end){
	IterCoord coord;
	while(iter != end){
		if(*iter == '\n'){
			++coord.line;
			coord.column = 0;
		} else {
			++coord.column;
		}
		
		if(iter == pos) return coord;
		++iter;
	}
	throw std::runtime_error("pos not in range [iter, end)");
}

}

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
	return Result<Iter>{ctx.max_iter, res, ctx.expecteds};
}


template<class Iter>
std::string error_msg(Iter begin, Iter end, const Result<Iter>& res){

	if(!res.expecteds.size())
		return "";
	
	const auto coord = detail::coord(begin, res.iter, end);
	std::stringstream ss;
	ss << coord.line << ':' << coord.column << ": ";
	ss << "expected ";
	if(res.expecteds.size() == 1){
		ss << '\'' <<  res.expecteds.front()->name() << '\'';
	}
	else{
		for(auto iter = res.expecteds.cbegin(); iter != res.expecteds.cend()-2; ++iter){
			ss << '\'' << (*iter)->name() << "', ";
		}
		ss << '\'' << (*(res.expecteds.cend()-2))->name() << "' or '" << (*(res.expecteds.cend()-1))->name() << '\'';
	}
	ss << " before '" << *res.iter << '\'';
	return ss.str();
}

}
}
