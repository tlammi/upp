#pragma once

#include <cstddef>

#include "upp/impl/math/detail/traits.hpp"
#include "upp/impl/math/detail/conv.hpp"

namespace upp{
namespace math{

template<class Wrap>
class Transpose{
public:

	using T = detail::mat_type_t<Wrap>;
	static constexpr size_t H = detail::mat_width_v<Wrap>;
	static constexpr size_t W = detail::mat_height_v<Wrap>;

	constexpr Transpose(){}

	constexpr Transpose(Wrap&& w): wrap_{std::forward<Wrap>(w)}{}

	constexpr T& operator()(size_t i, size_t j){
		return wrap_(j, i);
	}

	constexpr const T& operator()(size_t i, size_t j) const {
		return wrap_(j, i);
	}

private:
	Wrap wrap_;
};

template<class Wrap>
Transpose<Wrap> transpose(Wrap&& w){
	return {std::forward<Wrap>(w)};
}

}
}
