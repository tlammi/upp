#pragma once

#include <stdexcept>
#include <upp/math/detail/traits.hpp>
#include <vector>

namespace upp::math::detail {

consteval bool dimension_match(size_type a, size_type b) noexcept {
    if (a == dynamic || b == dynamic) return true;
    return a == b;
}

template <class T, class U>
consteval bool static_dimensions_match() noexcept {
    return dimension_match(T::static_height, U::static_height) &&
           dimension_match(T::static_width, U::static_width);
}

template <matrixlike T, matrixlike U>
constexpr auto& operator+=(T& a, const U& b) {
    static_assert(static_dimensions_match<T, U>(), "Matrix dimension mismatch");
    if (a.height() != b.height() || a.width() != b.width())
        throw std::out_of_range("matrix dimensions mismatch");
    for (auto i = size_type{}; i < b.height(); ++i) {
        for (auto j = size_type{}; j < b.width(); ++j) { a[i, j] += b[i, j]; }
    }
    return a;
}

template <matrixlike T, matrixlike U>
constexpr auto operator+(const T& a,
                         const U& b) noexcept(noexcept(std::declval<T&>() +=
                                                       std::declval<U>())) {
    auto out = a;
    out += b;
    return out;
}

template <matrixlike T, matrixlike U>
constexpr auto& operator-=(T& a, const U& b) {
    if (a.height() != b.height() || a.width() != b.width())
        throw std::out_of_range("matrix dimensions mismatch");
    for (auto i = size_type{}; i < b.height(); ++i) {
        for (auto j = size_type{}; j < b.width(); ++j) { a[i, j] -= b[i, j]; }
    }
    return a;
}

template <matrixlike T, matrixlike U>
constexpr auto operator-(const T& a, const U& b) noexcept(noexcept(a += b)) {
    auto out = a;
    out -= b;
    return out;
}

template <matrixlike T, matrixlike U>
constexpr auto& operator*=(T& a, const U& b) {
    auto h = a.height();
    auto w = b.width();
    if (a.width() != b.height())
        throw std::out_of_range("matrix dimensions mismatch");
    auto intermediate = a.width();

    auto vec = std::vector<typename T::value_type>(w);
    for (auto i = size_type{}; i < h; ++i) {
        for (auto j = size_type{}; j < w; ++j) {
            for (auto k = size_type{}; k < intermediate; ++k) {
                vec[j] += a[i, k] * b[k, j];
            }
        }
        for (auto j = size_type{}; j < w; ++j) {
            a[i, j] = vec[j];
            vec[j] = {};
        }
    }
    return a;
}

template <matrixlike T, matrixlike U>
constexpr auto operator*(const T& a, const U& b) {
    auto h = a.height();
    auto w = b.width();
    if (a.width() != b.height())
        throw std::out_of_range("matrix dimensions mismatch");

    auto intermediate = a.width();

    auto out = std::remove_cvref_t<T>::zeros(h, w);
    for (auto i = size_type{}; i < h; ++i) {
        for (auto j = size_type{}; j < h; ++j) {
            for (auto k = size_type{}; k < intermediate; ++k) {
                out[i, j] += a[i, k] * b[k, j];
            }
        }
    }
    return out;
}

}  // namespace upp::math::detail
