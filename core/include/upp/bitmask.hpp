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
    using raw_type = std::underlying_type_t<E>;
    raw_type m_v{};
    static constexpr auto zero = static_cast<raw_type>(0);

    constexpr explicit bitmask(raw_type v) noexcept : m_v(v){}

 public:
    constexpr bitmask() = default;
    constexpr explicit bitmask(E e) noexcept : bitmask(underlying_cast(e)) {}

    constexpr bool none() const noexcept {
        return m_v == zero;
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
        for (auto e : enums) {
            auto r = underlying_cast(e);
            auto bitcount = count_set_bits(r);
            if (bitcount != 1) continue;
            if (!(m_v & r)) return false;
        }
        return true;
    }

#else

    constexpr bool all() const noexcept {
        static constexpr auto enums = magic_enum::enum_values<E>();
        for (auto e : enums) {
            auto r = underlying_cast(e);
            auto bitcount = count_set_bits(r);
            if (bitcount != 1) continue;
            if (!(m_v & r)) return false;
        }
        return true;
    }

#endif

    constexpr bitmask operator|(bitmask other) const noexcept {
        return bitmask(m_v | other.m_v);
    }

    constexpr bitmask operator|(E other) const noexcept {
        return *this | bitmask(other);
    }


    constexpr bitmask operator&(bitmask other) const noexcept {
        return bitmask(m_v & other.m_v);
    }

    constexpr bitmask operator&(E other) const noexcept {
      return *this & bitmask(other);
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
