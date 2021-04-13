#pragma once

#include<vector>
#include<string_view>

namespace upp{
namespace parser{

template<class Iter>
struct Result{
	Iter iter;
	bool success;
	std::vector<std::string_view> misses{};
	explicit operator bool() const noexcept {
		return success;
	}
};
}
}
