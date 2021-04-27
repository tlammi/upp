#pragma once

namespace upp{
namespace math{

template<class L, class R>
constexpr bool operator==(L&& l, R&& r){
	
	constexpr size_t LH = detail::mat_height_v<L>;
	constexpr size_t RH = detail::mat_height_v<R>;
	constexpr size_t LW = detail::mat_width_v<L>;
	constexpr size_t RW = detail::mat_width_v<R>;
	
	static_assert(LH == RH);
	static_assert(LW == RW);

	for(size_t i=0; i < LH; ++i)
		for(size_t j=0; j < LW; ++j)
			if(l(i, j) != r(i, j)) return false;
	return true;
}


template<class L, class R>
constexpr bool operator!=(L&& l, R&& r){
	return !(l == r);
}

}
}
