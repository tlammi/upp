#pragma once

#include <coroutine>

namespace upp::async {

class Yielder {
 public:
    constexpr bool await_ready() const noexcept { return false; }

    constexpr void await_suspend(
        std::coroutine_handle<> /*unused*/) const noexcept {}

    constexpr void await_resume() {}

 private:
};

constexpr Yielder yield() { return {}; }

}  // namespace upp::async
