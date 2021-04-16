#pragma once

#include<deque>

#include "upp/impl/parser/ast.hpp"

namespace upp{
namespace parser{


template<class Iter>
struct Result{
	Iter iter;
	bool success;
	std::deque<const Ast<Iter>*> expecteds{};
	explicit operator bool() const noexcept {
		return success;
	}
};
}
}
