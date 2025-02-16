#pragma once

#include <coroutine>
#include <upp/util.hpp>

namespace upp::async {

class Yielder {
    UPP_ASSERT_CONDITION_NO_UNIQUE_ADDRESS mutable AssertCondition m_used{
        "upp::async::yield() not awaited"};

 public:
    constexpr bool await_ready() const noexcept {
        m_used = true;
        return false;
    }

    constexpr void await_suspend(
        std::coroutine_handle<> /*unused*/) const noexcept {}

    constexpr void await_resume() {}

 private:
};

[[nodiscard]] constexpr Yielder yield() { return {}; }

}  // namespace upp::async
