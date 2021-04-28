#pragma once

#include <cstddef>

#include "upp/impl/math/detail/traits.hpp"
#include "upp/impl/math/detail/conv.hpp"
#include "upp/impl/math/transpose.hpp"

namespace upp{
namespace math{

template<class T, size_t H, size_t W>
class DiagMat{
public:

	constexpr DiagMat(){}

	template<class Other>
	constexpr DiagMat(Other&& other){
		for(size_t i=0; i<std::min(H, W); ++i)
			operator()(i, i) = other(i, i);
	}

	constexpr T& operator()(size_t i, size_t j) {
		check_idx(i, j);
		if(i == j) return arr_[i];
		return dummy_;
	}

	constexpr const T& operator()(size_t i, size_t j) const {
		check_idx(i, j);
		if(i == j) return arr_[i];
		return dummy_;
	}

	Transpose<DiagMat&> t() noexcept {
		return {*this};
	}

	Transpose<const DiagMat&> t() const noexcept {
		return {*this};
	}

private:

	constexpr void check_idx(size_t i, size_t j) const {
		if(i>=H) throw std::runtime_error("Mat::operator(): i>H");
		if(j>=W) throw std::runtime_error("Mat::operator(): j>W");
	}
	T dummy_{};
	T arr_[std::min(H, W)];
};


template<class Other>
Mat(Other&& other) -> Mat<detail::mat_type_t<Other>, detail::mat_height_v<Other>, detail::mat_width_v<Other>>;

}
}
