#pragma once

#include <cassert>
#include <string_view>
#include <upp/bits/util/assert.hpp>
#include <upp/bits/util/macros.hpp>
#include <utility>

namespace upp {

class AssertCondition {
#if !defined(NDEBUG)
    bool m_ok{false};
    bool m_moved{false};
    std::string_view m_msg{};
#endif

 public:
    constexpr AssertCondition() noexcept = default;
    constexpr explicit AssertCondition(std::string_view msg) noexcept
#if !defined(NDEBUG)
        : m_msg(msg)
#endif
    {
    }

    constexpr AssertCondition(bool ok, std::string_view msg) noexcept
#if !defined(NDEBUG)
        : m_ok(ok),
          m_msg(msg)
#endif
    {
    }

    AssertCondition(const AssertCondition&) = delete;
    AssertCondition& operator=(const AssertCondition&) = delete;
    constexpr AssertCondition([[maybe_unused]] AssertCondition&& other) noexcept
#if !defined(NDEBUG)
        : m_ok(other.m_ok),
          m_moved(std::exchange(other.m_moved, true))
#endif
    {
    }

    constexpr AssertCondition& operator=(
        [[maybe_unused]] AssertCondition&& other) noexcept {
#if !defined(NDEBUG)
        m_ok = other.m_ok;
        m_moved = std::exchange(other.m_moved, true);
#endif
        return *this;
    }

    constexpr ~AssertCondition() { UPP_ASSERT(m_moved || m_ok, m_msg); }

    constexpr AssertCondition& operator=([[maybe_unused]] bool ok) noexcept {
        set(ok);
        return *this;
    }

    constexpr void set(bool ok) noexcept {
#if !defined(NDEBUG)
        m_ok = ok;
#endif
    }
};

#define UPP_ASSERT_CONDITION_NO_UNIQUE_ADDRESS \
    UPP_IF_NDEBUG([[no_unique_address]])

}  // namespace upp
