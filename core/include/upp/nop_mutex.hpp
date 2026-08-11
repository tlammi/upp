#pragma once

#include <upp/immovable.hpp>
#include <upp/uncopyable.hpp>

namespace upp {

/**
 * \brief Dummy mutex
 *
 * This is an utility class that can be used for templates or when no threads
 * are enabled.
 * */
class nop_mutex final : public immovable, public uncopyable {
 public:
    constexpr void lock() noexcept {}

    [[nodiscard]] constexpr bool try_lock() noexcept { return true; }

    constexpr void unlock() noexcept {}
};

}  // namespace upp
