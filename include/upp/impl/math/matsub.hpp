#pragma once

#include "upp/impl/math/detail/traits.hpp"

namespace upp{
namespace math{

template<class L, class R>
class MatSub{
public:
	static_assert(detail::mat_height_v<L> == detail::mat_height_v<R>);
	static_assert(detail::mat_width_v<L> == detail::mat_width_v<R>);

	using T = detail::mat_type_t<L>;
	static constexpr size_t H = detail::mat_height_v<L>;
	static constexpr size_t W = detail::mat_width_v<L>;

	constexpr MatSub(L&& l, R&& r): l_{std::forward<L>(l)}, r_{std::forward<R>(r)}{}

	constexpr T operator()(size_t i, size_t j) const {
		return l_(i, j) - r_(i, j);
	}

	operator Mat<T, H, W>() const {
		return detail::convert_mat<MatAdd<L, R>, Mat<T, H, W>>(*this);
	}

private:

	L l_;
	R r_;
};


template<class L, class R>
constexpr MatSub<L, R> operator-(L&& l, R&& r){
	return {std::forward<L>(l), std::forward<R>(r)};
}
}
}
