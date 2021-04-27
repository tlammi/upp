#pragma once

#include "upp/impl/math/detail/traits.hpp"

namespace upp{
namespace math{
namespace detail{


template<class From, class To, class = std::enable_if_t<is_mat_v<From> && is_mat_v<To>>>
To convert_mat(From&& from){
	constexpr size_t H = mat_height_v<From>;
	constexpr size_t W = mat_width_v<From>;
	
	To to;
	for(size_t i=0; i<H; ++i)
		for(size_t j=0; j<W; ++j)
			to(i, j) = from(i, j);
	return to;
}
}
}
}
