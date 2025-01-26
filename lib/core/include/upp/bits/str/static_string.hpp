#pragma once

#include <span>
#include <upp/bits/str/c_string.hpp>

namespace upp {

class StaticString {
    CString m_str{};

 public:
    consteval explicit StaticString(const char* s) noexcept : m_str(s) {}
    consteval explicit StaticString(const char* s, size_t len) noexcept
        : m_str(s, len) {}

    template <size_t S>
    consteval explicit StaticString(const char (&s)[S]) noexcept
        : m_str(s, S) {}

    constexpr StaticString(const StaticString&) = default;
    constexpr StaticString& operator=(const StaticString&) = default;

    constexpr StaticString(StaticString&&) noexcept = default;
    constexpr StaticString& operator=(StaticString&&) noexcept = default;

    constexpr operator std::string_view() const noexcept {
        return m_str.view();
    }
    constexpr operator CString() const noexcept { return m_str; }
};

namespace literals {

consteval StaticString operator""_static(const char* ptr, size_t len) noexcept {
    return StaticString{ptr, len};
}
}  // namespace literals

}  // namespace upp
