#pragma once

#include <concepts>
#include <limits>

namespace upp {

/**
 * \brief Integer with all bits set
 * */
template <std::integral T>
constexpr auto all_set() noexcept {
    if constexpr (std::unsigned_integral<T>) {
        return std::numeric_limits<T>::max();
    } else {
        return static_cast<T>(-1);
    }
}

/**
 * \brief Check how many bits in an integer are set.
 * */
template <std::integral T>
constexpr auto count_set_bits(T t) noexcept {
    static constexpr auto total_bits = sizeof(T) * 8;
    auto set_bits = 0u;
    for (auto i = 0u; i < total_bits; ++i) {
        const bool any_bit_set = t & (1 << i);
        if (any_bit_set) ++set_bits;
    }
    return set_bits;
}

}  // namespace upp
