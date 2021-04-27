#pragma once

#include "upp/impl/math/detail/traits.hpp"

namespace upp{
namespace math{

template<class L, class R>
class MatMul{
public:
	static_assert(detail::mat_width_v<L> == detail::mat_height_v<R>);

	using T = detail::mat_type_t<L>;
	static constexpr size_t H = detail::mat_height_v<L>;
	static constexpr size_t W = detail::mat_width_v<R>;

	constexpr MatMul(L&& l, R&& r): l_{std::forward<L>(l)}, r_{std::forward<R>(r)}{}

	constexpr T operator()(size_t i, size_t j) const {
		constexpr size_t M = detail::mat_width_v<L>;
		T res{};
		for(size_t k=0; k<M; ++k)
			res += l_(i, k)*r_(k, j);
		return res;
	}

	operator Mat<T, H, W>() const {
		return detail::convert_mat<MatAdd<L, R>, Mat<T, H, W>>(*this);
	}

private:

	L l_;
	R r_;
};


template<class L, class R>
constexpr MatMul<L, R> operator*(L&& l, R&& r){
	return {std::forward<L>(l), std::forward<R>(r)};
}
}
}
