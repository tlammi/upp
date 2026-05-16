#pragma once

namespace upp {

class uncopyable {
 public:
    uncopyable(const uncopyable&) noexcept = delete;
    uncopyable& operator=(const uncopyable&) noexcept = delete;

    uncopyable(uncopyable&&) noexcept = default;
    uncopyable& operator=(uncopyable&&) noexcept = default;

 protected:
    constexpr uncopyable() noexcept = default;
    constexpr ~uncopyable() noexcept = default;
};
}  // namespace upp
