#pragma once

#include <cstddef>

#include "upp/impl/math/detail/traits.hpp"
#include "upp/impl/math/detail/conv.hpp"

namespace upp{
namespace math{

template<class T, size_t H, size_t W>
class Mat{
public:

	constexpr Mat(){}

	template<class Other>
	constexpr Mat(Other&& other){
		for(size_t i=0; i<H; ++i)
			for(size_t j=0; j<W; ++j)
				operator()(i, j) = other(i, j);
	}

	constexpr T& operator()(size_t i, size_t j){
		check_idx(i, j);
		return arr_[i][j];
	}

	constexpr const T& operator()(size_t i, size_t j) const {
		check_idx(i, j);
		return arr_[i][j];
	}

private:

	constexpr void check_idx(size_t i, size_t j) const {
		if(i>=H) throw std::runtime_error("Mat::operator(): i>H");
		if(j>=W) throw std::runtime_error("Mat::operator(): j>W");
	}
	T arr_[H][W];
};


template<class Other>
Mat(Other&& other) -> Mat<detail::mat_type_t<Other>, detail::mat_height_v<Other>, detail::mat_width_v<Other>>;

}
}
