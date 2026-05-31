#pragma once

#include <upp/bitset.hpp>
#include <upp/cast.hpp>
#include <upp/concepts.hpp>
#include <upp/enum.hpp>

namespace upp {

/**
 * \brief Set type for enumerators
 *
 * Efficient type for storing enumerations in a set. This is meant for
 * enumerations that are not "bitmask like", for those types one should
 * use \a upp::bitmask instead.
 * */
template <enum_type E>
class enum_set {
 public:
    constexpr enum_set() noexcept = default;
    constexpr explicit enum_set(std::initializer_list<E> il) noexcept {
        for (auto e : il) { *this |= e; }
    }

    constexpr explicit operator bool() const noexcept { return m_set.any(); }

    constexpr auto& operator|=(E v) noexcept {
        m_set[*enum_index(v)] = true;
        return *this;
    }

    constexpr auto operator|(E v) const noexcept {
        auto new_set = *this;
        new_set |= v;
        return new_set;
    }

    constexpr bool contains(E v) const noexcept {
        return m_set[*enum_index(v)];
    }

 private:
    bitset<enum_count<E>()> m_set{};
};
}  // namespace upp
