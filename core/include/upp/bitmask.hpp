#pragma once

#include <upp/cast.hpp>
#include <upp/concepts.hpp>
#include <upp/detail/feature.hpp>
#include <upp/enum.hpp>
#include <upp/int.hpp>

#if !UPP_HAVE_REFLECTION
#include <upp/thirdparty/magic_enum.hpp>
#endif

namespace upp {

/**
 * \brief Utility type for providing operators for bitmask-like enums
 *
 * This type provides all bitwise operators for enums classes that typically
 * would require implementing the operators for each type individually.
 *
 * */
template <enum_type E>
class bitmask {
    E m_v{};
    using raw_type = std::underlying_type_t<E>;
    static constexpr auto zero = static_cast<raw_type>(0);
    static constexpr auto all_bits = all_set<raw_type>();

 public:
    constexpr bitmask() = default;
    constexpr explicit bitmask(E e) noexcept : m_v(e) {}

    constexpr bool none() const noexcept {
        return underlying_cast(m_v) == zero;
    }

    /**
     * \brief Check whether any bits in the mask are set
     * */
    constexpr bool any() const noexcept { return !none(); }

#if UPP_HAVE_REFLECTION
    /**
     * \brief Check whether all the bits in the mask are set
     *
     * This works by iterating all the enum values with a single bit set and
     * checks that those bits are set in the mask. Possible enum values with
     * multiple bits set are skipped.
     * */
    constexpr bool all() const noexcept
        requires(detail::have_reflection)
    {
        static constexpr auto enums = enum_values<E>();
        // TODO: Check if this needs optimization, i.e. whether the compiler is
        // smart enough.
        const auto raw = underlying_cast(m_v);
        for (auto e : enums) {
            auto r = underlying_cast(e);
            auto bitcount = count_set_bits(r);
            if (bitcount != 1) continue;
            if (!(raw & r)) return false;
        }
        return true;
    }

#else

    constexpr bool all() const noexcept {
        static constexpr auto enums = magic_enum::enum_values<E>();
        const auto raw = underlying_cast(m_v);
        for (auto e : enums) {
            auto r = underlying_cast(e);
            auto bitcount = count_set_bits(r);
            if (bitcount != 1) continue;
            if (!(raw & r)) return false;
        }
        return true;
    }

#endif
    constexpr bitmask operator|(E other) const noexcept {
        return bitmask(
            underlying_cast<E>(underlying_cast(m_v) | underlying_cast(other)));
    }

    constexpr bitmask operator|(bitmask other) const noexcept {
        return *this | other.m_v;
    }

    constexpr bitmask operator&(E other) const noexcept {
        return bitmask(
            underlying_cast<E>(underlying_cast(m_v) & underlying_cast(other)));
    }

    constexpr bitmask operator&(bitmask other) const noexcept {
        return *this & other.m_v;
    }
};

namespace detail {
class bitmask_builder {
 public:
    template <enum_type E>
    constexpr bitmask<E> operator|(E e) const noexcept {
        return bitmask(e);
    }
};
}  // namespace detail

constexpr auto bm = detail::bitmask_builder();

}  // namespace upp
