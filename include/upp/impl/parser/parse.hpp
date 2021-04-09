#pragma once

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
Result<Iter> parse(Iter begin, Iter end, A&& ast, Iter*(skipper)(Iter, Iter)=nullptr){
	return ast.match(begin, end, skipper);
}

}
}
