#pragma once

namespace upp {

class immovable {
 public:
    immovable(const immovable&) noexcept = default;
    immovable& operator=(const immovable&) noexcept = default;

    immovable(immovable&&) = delete;
    immovable& operator=(immovable&&) = delete;

 protected:
    constexpr immovable() noexcept = default;
    constexpr ~immovable() noexcept = default;
};
}  // namespace upp
