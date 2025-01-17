#pragma once

#include <cstddef>
#include <string>
#include <string_view>

namespace upp {

/**
 * \brief String view pointing to a string literal
 * */
class strlit {
    std::string_view m_s{""};

 public:
    constexpr strlit() noexcept = default;
    template <std::size_t N>
    consteval strlit(const char (&s)[N] /*NOLINT*/) : m_s(s, N - 1) {}

    consteval strlit(const char* s /*NOLINT*/) : m_s(s) {}

    constexpr strlit(const strlit&) noexcept = default;
    constexpr strlit& operator=(const strlit&) noexcept = default;

    constexpr strlit(strlit&&) noexcept = default;
    constexpr strlit& operator=(strlit&&) noexcept = default;

    constexpr ~strlit() = default;

    constexpr const char* c_str() const noexcept { return m_s.data(); }

    constexpr explicit operator std::string_view() const noexcept {
        return m_s;
    }

    constexpr explicit operator const char*() const noexcept {
        return m_s.data();
    }

    constexpr explicit operator std::string() const noexcept {
        return std::string(m_s);
    }

    constexpr auto operator<=>(const strlit&) const noexcept = default;

    constexpr bool operator==(std::string_view other) const noexcept {
        return m_s == other;
    }

    constexpr auto operator<=>(std::string_view other) const noexcept {
        return m_s <=> other;
    }

    constexpr bool empty() const noexcept { return m_s.empty(); }

    constexpr auto size() const noexcept { return m_s.size(); }
    constexpr auto length() const noexcept { return m_s.length(); }

    constexpr auto begin() const noexcept { return m_s.begin(); }
    constexpr auto end() const noexcept { return m_s.end(); }
    constexpr auto rbegin() const noexcept { return m_s.rbegin(); }
    constexpr auto rend() const noexcept { return m_s.rend(); }

    constexpr auto cbegin() const noexcept { return m_s.cbegin(); }
    constexpr auto cend() const noexcept { return m_s.cend(); }
    constexpr auto crbegin() const noexcept { return m_s.crbegin(); }
    constexpr auto crend() const noexcept { return m_s.crend(); }
};

namespace literals {
consteval auto operator""_lit(const char* s, unsigned long /*len*/) noexcept {
    return strlit(s);
}
}  // namespace literals
}  // namespace upp
