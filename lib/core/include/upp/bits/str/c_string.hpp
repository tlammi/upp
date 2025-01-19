#pragma once

#include <cstddef>
#include <cstring>
#include <string>
#include <upp/bits/str/util.hpp>

namespace upp {

/**
 * \brief std::string_view with guaranteed null termination
 * */
class CString {
  const char* m_ptr{};
  size_t m_len{};

 public:
  constexpr CString() noexcept = default;
  constexpr CString(const char* str) noexcept
      : CString(str, upp::strlen(str)) {}

  template <size_t S>
  constexpr CString(const char (&str)[S]) noexcept : CString(str, S) {}

  constexpr CString(const char* str, size_t len) noexcept
      : m_ptr(str), m_len(len) {}

  constexpr CString(const std::string& str) noexcept
      : CString(str.c_str(), str.size()) {}

  constexpr const char* c_str() const noexcept { return m_ptr; }
  constexpr const char* data() const noexcept { return m_ptr; }
  constexpr std::string_view view() const noexcept { return {m_ptr, m_len}; }
  constexpr size_t size() const noexcept { return m_len; }
  constexpr size_t length() const noexcept { return m_len; }

  constexpr CString substr(size_t offset) const noexcept {
    return {m_ptr + offset, m_len - offset};
  }

  constexpr void remove_prefix(size_t count) noexcept {
    m_ptr += count;
    m_len -= count;
  }

  constexpr operator const char*() const noexcept { return m_ptr; }

  constexpr explicit operator std::string_view() const noexcept {
    return {m_ptr, m_len};
  }

  constexpr auto operator<=>(CString other) const noexcept {
    return view() <=> other.view();
  }
  constexpr auto operator==(CString other) const noexcept {
    return (*this <=> other) == std::strong_ordering::equal;
  }

  constexpr auto operator<=>(std::string_view other) const noexcept {
    return view() <=> other;
  }
  constexpr auto operator==(std::string_view other) const noexcept {
    return (*this <=> other) == std::strong_ordering::equal;
  }

  constexpr auto operator<=>(const std::string& other) const noexcept {
    return view() <=> other;
  }
  constexpr auto operator==(const std::string& other) const noexcept {
    return (*this <=> other) == std::strong_ordering::equal;
  }

  constexpr auto operator<=>(const char* str) const noexcept {
    return upp::strcmp_ordering(m_ptr, str);
  }
  constexpr auto operator==(const char* str) const noexcept {
    return (*this <=> str) == std::strong_ordering::equal;
  }
};

namespace literals {
constexpr CString operator""_c_str(const char* str, size_t len) {
  return {str, len};
}

}  // namespace literals

}  // namespace upp
