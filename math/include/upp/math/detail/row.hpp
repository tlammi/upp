#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <upp/type_traits.hpp>

namespace upp::math::detail {

template <class T, std::size_t N>
struct row {
    std::array<T, N> vals;

    template <class... Us>
    row(Us... us) : vals{std::forward<Us>(us)...} {}
};

template <class... Ts>
row(Ts...) -> row<upp::first_of_t<Ts...>, sizeof...(Ts)>;

template <class T, std::size_t N>
constexpr std::size_t row_len(const row<T, N>&) noexcept {
    return N;
}

template <class T, class... Ts>
constexpr std::size_t max_row_len(const T& first, const Ts&... rows) noexcept {
    if constexpr (sizeof...(rows) == 0)
        return row_len(first);
    else
        return std::max(row_len(first), max_row_len(rows...));
}

template <class T>
struct is_row : std::false_type {};

template <class T, std::size_t N>
struct is_row<row<T, N>> : std::true_type {};

template <class T>
constexpr bool is_row_v = is_row<std::remove_cvref_t<T>>::value;

void assign_rows(auto rows_out, std::size_t i, auto&& row_in,
                 auto&&... rows_in) {
    assert(rows_out[i].size() >= row_in.vals.size());
    std::ranges::copy(row_in.vals, rows_out[i].begin());
    if constexpr (sizeof...(rows_in))
        assign_rows(rows_out, i + 1, UPP_FWD(rows_in)...);
}

}  // namespace upp::math::detail
