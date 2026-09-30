#pragma once

#include <concepts>

namespace upp {

/**
 * \brief Safe boolean type
 *
 * Replacement for builtin bool type blocking accidental
 * implicit conversions to bool.
 *
 * */
class sbool {
    bool m_v{};

 public:
    constexpr sbool() noexcept = default;
    constexpr sbool(bool b) noexcept : m_v(b) {}

    sbool(const char*) = delete ("Trying to convert cstring to bool");
    sbool(std::floating_point auto) =
        delete ("Implicit conversion from float to boolean");
    sbool(std::integral auto) =
        delete ("Implicit conversion from integer to boolean");

    constexpr explicit operator bool() const noexcept { return m_v; }

    // NOLINTNEXTLINE
#define OPER(oper)                                                     \
    constexpr sbool operator oper(const sbool& other) const noexcept { \
        return m_v oper other.m_v;                                     \
    }

    OPER(==)
    OPER(!=)
    OPER(<=)
    OPER(>=)
    OPER(<)
    OPER(>)

#undef OPER
};
}  // namespace upp
