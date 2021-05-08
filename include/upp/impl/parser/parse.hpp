#pragma once

#include "upp/impl/parser/result.hpp"

namespace upp{
namespace parser{

namespace detail{

/// Store coordinates in sequence
struct IterCoord{
	size_t line{1};
	size_t column{0};
};

/**
 * Convert iterator to line and column coordinates
 *
 * \param iter Sequence start
 * \param pos Position to convert to coordinates
 * \param end Sequence end
 * \return Coordinates of the \ref pos in sequence
 */
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
	return coord;
}

}

/// Skipper for skipping all whitespaces between lexemes
auto whitespaces(auto iter, auto end){
	while(iter != end && std::isspace(*iter)) ++iter;
	return iter;
}

/// Skipper for skipping all spaces between lexemes
auto spaces(auto iter, auto end){
	while(iter != end && *iter == ' ') ++iter;
	return iter;
}

/**
 * Parse sequence using the given grammar
 *
 * \param begin Sequence start
 * \param end Sequence end
 * \param ast Root Ast specifying a grammar
 * \param skipper Skipping function called between Asts to skip characters.
 * 	This is optional and if not specified, no skipping is performed.
 * 	The skipper is greedy, so e.g. \ref parser::whitespaces will consume
 * 	all whitespaces before a non-whitespace character.
 *
 * \return Parsing result. Evaluates to true if valid, false otherwise
 */
template<class Iter, class A>
Result<Iter> parse(Iter begin, Iter end, A&& ast, Iter(*skipper)(Iter, Iter)=nullptr){
	detail::Ctx<Iter> ctx{begin, end, skipper};
	bool res = detail::match(ast, ctx);
	for(const auto [ptr, mbegin, mend]: ctx.matches){
		detail::invoke(*ptr, mbegin, mend);
	}
	return Result<Iter>{ctx.max_iter, res, ctx.expecteds};
}


/**
 * Inspect parsing results to create an error message
 *
 * Used for getting a verbose status of the parsing.
 *
 * \param begin Sequence start also passed to \ref parse
 * \param end Sequence end also passed to \ref parse
 * \param res Return value of \ref parse
 */
template<class Iter>
std::string error_msg(Iter begin, Iter end, const Result<Iter>& res){

	std::vector<const Ast<Iter>*> expecteds;
	for(const auto* ast: res.expecteds)
		expecteds.push_back(ast);
	std::sort(expecteds.begin(), expecteds.end());
	expecteds.erase(std::unique(expecteds.begin(), expecteds.end()), expecteds.end());

	if(!res.expecteds.size())
		return "";
	
	const auto coord = detail::coord(begin, res.iter, end);
	std::stringstream ss;
	ss << coord.line << ':' << coord.column << ": ";
	ss << "expected ";
	if(res.expecteds.size() == 1){
		ss << '\'' <<  (*expecteds.begin())->name() << '\'';
	}
	else{
		for(auto iter = expecteds.cbegin(); iter != expecteds.cend()-2; ++iter){
			ss << '\'' << (*iter)->name() << "', ";
		}
		ss << '\'' << (*(expecteds.cend()-2))->name() << "' or '" << (*(expecteds.cend()-1))->name() << '\'';
	}
	if(res.iter != end)
		ss << " before '" << *res.iter << '\'';
	else
		ss << " before end of stream";
	return ss.str();
}

}
}
