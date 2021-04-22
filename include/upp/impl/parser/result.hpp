#pragma once

#include<deque>

#include "upp/impl/parser/ast.hpp"

namespace upp{
namespace parser{

/**
 * Parsing result
 */
template<class Iter>
struct Result{
	Iter iter; ///< Where iterator was left after parsing
	bool success; ///< True, if success, false otherwise

	/// Contains pointer to \ref Ast instances that were
	/// last tried to be matched. Used in \ref error_msg
	/// function mainly
	std::deque<const Ast<Iter>*> expecteds{};

	explicit operator bool() const noexcept {
		return success;
	}
};
}
}
