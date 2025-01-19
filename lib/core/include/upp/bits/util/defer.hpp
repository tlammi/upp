#pragma once

#include <type_traits>
#include <utility>

namespace upp {

template <class F>
class [[nodiscard]] Defer {
  F m_f;
  bool m_cancelled{false};

  static_assert(std::is_nothrow_invocable_v<F>,
                "Function is called in destructor so it cannot throw");

 public:
  template <class T>
  constexpr explicit Defer(T&& t) noexcept : m_f(std::forward<T>(t)) {}

  Defer(const Defer&) = delete;
  Defer& operator=(const Defer&) = delete;

  constexpr Defer(Defer&& other) noexcept
      : m_f(std::move(other.m_f)),
        m_cancelled(std::exchange(other.m_cancelled, true)) {}

  Defer& operator=(Defer&&) = delete;

  ~Defer() {
    if (!m_cancelled) m_f();
  }

  constexpr void cancel(bool cancelled = true) noexcept {
    m_cancelled = cancelled;
  }

  constexpr bool cancelled() const noexcept { return m_cancelled; }
};

template <class T>
Defer(T) -> Defer<T>;

}  // namespace upp
