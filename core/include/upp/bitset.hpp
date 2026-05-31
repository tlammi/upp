#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <ranges>
#include <string_view>
#include <upp/immovable.hpp>
#include <upp/uncopyable.hpp>

namespace upp {

/**
 * \brief Bitset type
 *
 * Provides a custom width bitset
 * */
template <std::size_t N>
class bitset {
    static constexpr auto arr_size = N / 8 + 1;

    using array_type = std::array<std::uint8_t, arr_size>;

 public:
    constexpr bitset() noexcept = default;

    consteval explicit bitset(std::string_view s) noexcept {
        static constexpr auto bits_in_byte = 8;
        if (s.starts_with("0b")) {
            s.remove_prefix(2);
            size_t byte_idx = 0;
            size_t bit_idx = 0;
            for (auto bit : s | std::views::reverse) {
                if (bit == '0') {
                    // nop
                } else if (bit == '1') {
                    m_arr[byte_idx] |= 1 << bit_idx;
                    if (bit_idx == bits_in_byte) {
                        bit_idx = 0;
                        ++byte_idx;
                    }
                } else {
                    assert(false && "not a valid bitmask string");
                }
                ++bit_idx;
            }
            return;
        }
        if (s.starts_with("0x")) {
            s.remove_prefix(2);
            size_t byte_idx = 0;
            bool top_half = false;
            for (auto c : s | std::views::reverse) {
                uint8_t val = 0;
                if (c >= '0' && c <= '9')
                    val = c - '0';
                else if (c >= 'a' && c <= 'f')
                    val = c - 'a' + 10;  // NOLINT
                else if (c >= 'A' && c <= 'F')
                    val = c - 'A' + 10;  // NOLINT
                else
                    assert(false && "not a valid bitmask string");
                if (top_half) {
                    m_arr[byte_idx] |= val << 4;
                    ++byte_idx;
                } else
                    m_arr[byte_idx] |= val;
                top_half = !top_half;
            }
            return;
        }
        assert(false && "unknown prefix include bitset()");
    }

    constexpr explicit operator bool() const noexcept { return any(); }

    constexpr bool any() const noexcept {
        for (auto byte : m_arr)
            if (byte) return true;
        return false;
    }

    constexpr bool none() const noexcept { return !any(); }

    constexpr bool all() const noexcept {
        static constexpr std::uint8_t all_bits = 0xff;
        for (auto byte : m_arr)
            if (byte != all_bits) return false;
        return true;
    }

    constexpr bool operator[](std::size_t idx) const noexcept {
        const auto byte = idx / 8;
        const auto bit = idx % 8;
        return m_arr[byte] & (1 << bit);
    }

    class index_proxy : uncopyable, immovable {
        friend bitset;
        std::size_t m_byte;
        std::size_t m_bit;
        array_type* m_arr;

        index_proxy(std::size_t byte, std::size_t bit, array_type* arr) noexcept
            : m_byte(byte), m_bit(bit), m_arr(arr) {}

     public:
        constexpr auto& operator=(bool b) noexcept {
            if (b)
                (*m_arr)[m_byte] |= 1 << m_bit;
            else
                (*m_arr)[m_byte] &= ~(1 << m_bit);
            return *this;
        }

        constexpr bool operator==(bool b) const noexcept {
            bool bit_set = (*m_arr)[m_byte] & 1 << m_bit;
            return bit_set == b;
        }
    };

    constexpr index_proxy operator[](size_t idx) noexcept {
        const auto byte = idx / 8;
        const auto bit = idx % 8;
        return {byte, bit, &m_arr};
    }

    constexpr std::uint8_t byte(size_t idx) const noexcept {
        return m_arr[idx];
    }

    constexpr std::uint8_t& byte(size_t idx) noexcept { return m_arr[idx]; }

 private:
    array_type m_arr{};
};

}  // namespace upp
