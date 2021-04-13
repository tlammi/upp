#pragma once

#include <vector>

namespace upp{
namespace parser{


template<class Iter>
struct Ctx{
	using Skipper = Iter(*)(Iter, Iter);
	Iter iter;
	const Iter end;
	const Skipper skipper;
	std::vector<std::string_view> misses{};
};
}
}
