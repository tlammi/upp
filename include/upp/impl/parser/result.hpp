#pragma once

namespace upp{
namespace parser{

template<class Iter>
struct Result{
	Iter iter;
	bool success;
	explicit operator bool() const noexcept {
		return success;
	}
};
}
}
