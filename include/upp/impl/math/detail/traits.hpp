#pragma once

#include <cstddef>

namespace upp{
namespace math{

template<class T, size_t H, size_t W>
class Mat;

template<class L, class R>
class MatAdd;

template<class L, class R>
class MatSub;

template<class L, class R>
class MatMul;

namespace detail{

template<class T>
struct is_mat: std::false_type{};

template<class T, size_t H, size_t W>
struct is_mat<Mat<T, H, W>>: std::true_type{};

template<class L, class R>
struct is_mat<MatAdd<L, R>>: std::true_type{};

template<class L, class R>
struct is_mat<MatSub<L, R>>: std::true_type{};

template<class L, class R>
struct is_mat<MatMul<L, R>>: std::true_type{};

template<class T>
constexpr bool is_mat_v = is_mat<std::decay_t<T>>::value;


template<class T>
struct mat_type{};

template<class T>
using mat_type_t = typename mat_type<std::decay_t<T>>::type;

template<class T, size_t H, size_t W>
struct mat_type<Mat<T, H, W>>{
	using type = T;
};

template<class L, class R>
struct mat_type<MatAdd<L, R>>{
	using type = mat_type_t<L>;
};

template<class L, class R>
struct mat_type<MatSub<L, R>>{
	using type = mat_type_t<L>;
};

template<class L, class R>
struct mat_type<MatMul<L, R>>{
	using type = mat_type_t<L>;
};


template<class T>
struct mat_height{};

template<class T>
constexpr size_t mat_height_v = mat_height<std::decay_t<T>>::value;

template<class T, size_t H, size_t W>
struct mat_height<Mat<T, H, W>>{
	static constexpr size_t value = H;
};

template<class L, class R>
struct mat_height<MatAdd<L, R>>{
	static constexpr size_t value = mat_height_v<L>;
};

template<class L, class R>
struct mat_height<MatSub<L, R>>{
	static constexpr size_t value = mat_height_v<L>;
};

template<class L, class R>
struct mat_height<MatMul<L, R>>{
	static constexpr size_t value = mat_height_v<L>;
};


template<class T>
struct mat_width{};

template<class T>
constexpr size_t mat_width_v = mat_width<std::decay_t<T>>::value;

template<class T, size_t H, size_t W>
struct mat_width<Mat<T, H, W>>{
	static constexpr size_t value = W;
};

template<class L, class R>
struct mat_width<MatAdd<L, R>>{
	static constexpr size_t value = mat_width_v<L>;
};

template<class L, class R>
struct mat_width<MatSub<L, R>>{
	static constexpr size_t value = mat_width_v<L>;
};

template<class L, class R>
struct mat_width<MatMul<L, R>>{
	static constexpr size_t value = mat_width_v<R>;
};

}
}
}

