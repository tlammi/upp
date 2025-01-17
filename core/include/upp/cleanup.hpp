#pragma once

#include <exception>
#include <type_traits>

namespace upp {

namespace cleanup_detail {
template <class Fn, class Traits>
class [[nodiscard]] basic_cleanup {
 public:
    constexpr explicit basic_cleanup(Fn fn) noexcept(
        std::is_nothrow_move_constructible_v<Fn>)
        : m_fn(fn) {}

    basic_cleanup(const basic_cleanup&) = delete;
    basic_cleanup& operator=(const basic_cleanup&) = delete;

    basic_cleanup(basic_cleanup&&) = delete;
    basic_cleanup& operator=(basic_cleanup&&) = delete;

    constexpr void cancel(bool cancelled = true) noexcept {
        m_cancelled = cancelled;
    }

    constexpr ~basic_cleanup() {
        if (m_cancelled) return;
        if (!Traits::need_cleanup()) return;
        m_fn();
    }

 private:
    Fn m_fn;
    bool m_cancelled{false};
};

struct always_traits {
    static constexpr bool need_cleanup() noexcept { return true; }
};
struct throw_traits {
    static constexpr bool need_cleanup() noexcept {
        return std::uncaught_exceptions() > 0;
    }
};
struct success_traits {
    static constexpr bool need_cleanup() noexcept {
        return std::uncaught_exceptions() == 0;
    }
};

}  // namespace cleanup_detail

template <class Fn>
using cleanup =
    cleanup_detail::basic_cleanup<Fn, cleanup_detail::always_traits>;

template <class Fn>
using cleanup_throw =
    cleanup_detail::basic_cleanup<Fn, cleanup_detail::throw_traits>;

template <class Fn>
using cleanup_success =
    cleanup_detail::basic_cleanup<Fn, cleanup_detail::success_traits>;

}  // namespace upp
