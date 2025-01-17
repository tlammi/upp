#pragma once

#include <compare>
#include <cstddef>
#include <cstring>

namespace upp {

constexpr size_t strlen(const char* s) {
  size_t idx = 0;
  while (s[idx]) ++idx;
  return idx;
}

constexpr int strcmp(const char* lhs, const char* rhs) {
  if consteval {
    size_t i = 0;
    while (true) {
      if (!lhs[i] && !rhs[i]) return 0;
      if (lhs[i] < rhs[i]) return -1;
      if (lhs[i] > rhs[i]) return 1;
      ++i;
    }
  } else {
    return ::strcmp(lhs, rhs);
  }
}

constexpr std::strong_ordering strcmp_ordering(const char* lhs,
                                               const char* rhs) noexcept {
  int i = strcmp(lhs, rhs);
  if (i > 0) return std::strong_ordering::greater;
  if (i < 0) return std::strong_ordering::less;
  return std::strong_ordering::equal;
}

}  // namespace upp
