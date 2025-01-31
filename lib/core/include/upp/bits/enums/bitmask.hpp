#pragma once

#include <concepts>
#include <magic_enum/magic_enum_utility.hpp>
#include <upp/bits/enums/cast.hpp>
#include <upp/concepts.hpp>

namespace upp {

template <concepts::enum_bitmask E>
class BitMask {
 public:
    using enum_type = E;
    using value_type = std::underlying_type_t<enum_type>;

    constexpr explicit BitMask(value_type v) noexcept : m_val(v) {}

    template <std::convertible_to<E>... Es>
    constexpr explicit BitMask(Es... e) noexcept
        : m_val((0 | ... | underlying_cast(e))) {}

    constexpr BitMask operator|(BitMask other) const noexcept {
        return BitMask(m_val | other.m_val);
    }

    constexpr BitMask operator|(E e) const noexcept {
        return BitMask(m_val | underlying_cast(e));
    }

    constexpr BitMask operator&(BitMask other) const noexcept {
        return BitMask(m_val & other.m_val);
    }

    constexpr BitMask operator&(enum_type e) const noexcept {
        return BitMask(m_val & underlying_cast(e));
    }

    constexpr BitMask operator^(BitMask other) const noexcept {
        return m_val ^ other.m_val;
    }

    constexpr BitMask operator~() const noexcept { return ~m_val; }

    constexpr bool none() const noexcept { return !m_val; }
    constexpr bool any() const noexcept { return m_val; }
    constexpr bool all() const noexcept {
        bool all_ones = true;
        magic_enum::enum_for_each<E>([&](E e) {
            if (!all_ones) return;
            if (!(m_val & underlying_cast(e))) all_ones = false;
        });
        return all_ones;
    }

    constexpr explicit operator bool() const noexcept { return any(); }

    constexpr explicit operator value_type() const noexcept { return m_val; }

 private:
    value_type m_val{};
};

}  // namespace upp
