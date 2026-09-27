#pragma once

namespace upp {

[[noreturn]] void throw_errno(int ec);
[[noreturn]] void throw_errno();

// NOLINTNEXTLINE
#define UPP_CHECK(condition, ex)   \
    if (!(condition)) [[unlikely]] \
    throw(ex)
}  // namespace upp
