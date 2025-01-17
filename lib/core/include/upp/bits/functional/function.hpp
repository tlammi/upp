#pragma once

#include <memory>
#include <upp/type_traits.hpp>
#include <utility>

namespace upp {

template <class T>
class Function;

/**
 * \brief Type erased callable
 *
 * Behaves like std::function with additional support for easy conversion to
 * function pointer + userdata.
 * */
template <class R, class... Ps>
class Function<R(Ps...)> {
  class Ctx {
   public:
    constexpr virtual ~Ctx() = default;
    constexpr virtual R operator()(Ps...) = 0;
  };

  template <class F>
  class CtxImpl final : public Ctx {
    mutable F m_f;

   public:
    constexpr CtxImpl(F&& f) : m_f(std::move(f)) {}

    constexpr R operator()(Ps... ps) final {
      return m_f(std::forward<Ps>(ps)...);
    }
  };

  std::unique_ptr<Ctx> m_ctx{};

 public:
  /**
   * \brief Construct an empty Function. Contains a nullptr.
   * */
  constexpr Function() noexcept = default;

  constexpr explicit Function(std::nullptr_t /*unused*/) noexcept {}

  /**
   * \brief Construct a Function containing the object
   *
   * \param F Callable to contain
   * */
  template <class F>
  constexpr Function(F&& f) noexcept
      : m_ctx(std::make_unique<CtxImpl<F>>(std::move(f))) {}

  Function(const Function&) = delete;
  Function& operator=(const Function&) = delete;
  Function(Function&&) = default;
  Function& operator=(Function&&) = default;

  constexpr ~Function() = default;

  /**
   * \brief False if contains a nullptr, true otherwise
   * */
  constexpr explicit operator bool() const noexcept { return m_ctx.get(); }

  constexpr R operator()(Ps... ps) const {
    return (*m_ctx)(std::forward<Ps>(ps)...);
  }

  constexpr auto callback() const noexcept -> R (*)(void*, Ps...) {
    if (!m_ctx) return nullptr;
    return +[](void* ptr, Ps... ps) {
      auto* ctx = static_cast<Ctx*>(ptr);
      return (*ctx)(std::forward<Ps>(ps)...);
    };
  }

  constexpr auto callback2() const noexcept -> R (*)(Ps..., void*) {
    if (!m_ctx) return nullptr;
    return +[](Ps... ps, void* ptr) {
      auto* ctx = static_cast<Ctx*>(ptr);
      return (*ctx)(std::forward<Ps>(ps)...);
    };
  }

  constexpr void* userdata() const noexcept {
    return static_cast<void*>(m_ctx.get());
  }
};

template <class T>
Function(T) -> Function<prototype_of_t<T>>;

}  // namespace upp
