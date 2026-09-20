#pragma once

#include <concepts>
#include <limits>

namespace upp::math::detail {

using size_type = std::size_t;
constexpr auto dynamic = std::numeric_limits<size_type>::max();

template <class M>
concept matrixlike = requires(M m, size_type s) {
    //{ M::static_height } -> std::convertible_to<size_type>;
    //{ M::static_width } -> std::convertible_to<size_type>;
    typename M::value_type;
    { m[s, s] };
    { m.height() } -> std::convertible_to<size_type>;
    { m.width() } -> std::convertible_to<size_type>;
};

struct flattend_index_t {
    struct args {
        size_type h;  // not used
        size_type w;
        size_type i;
        size_type j;
    };

    constexpr auto operator()(args a) const noexcept {
        return std::size_t{a.w} * a.i + a.j;
    }
};

constexpr auto flatten_index = flattend_index_t{};
}  // namespace upp::math::detail
