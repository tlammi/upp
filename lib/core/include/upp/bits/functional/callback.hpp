#pragma once

#include <upp/bits/functional/function.hpp>
#include <upp/bits/functional/static_function.hpp>

namespace upp {

template <class Proto>
class Callback;

template <class R, class... Ps>
class Callback<R(Ps...)> {
    void* m_userdata{};
    R (*m_invoke)(void*, Ps...){};

 public:
    constexpr Callback() = default;
    constexpr Callback(std::nullptr_t /*unused*/) noexcept {}

    constexpr Callback(const Function<R(Ps...)>& f)
        : m_userdata(f.userdata()), m_invoke(f.callback()) {}

    template <size_t S>
    constexpr Callback(StaticFunction<R(Ps...), S>& f)
        : m_userdata(f ? f.userdata() : nullptr), m_invoke(f.callback()) {}

    constexpr R operator()(Ps... ps) const noexcept {
        return m_invoke(m_userdata, std::forward<Ps>(ps)...);
    }

    constexpr explicit operator bool() const noexcept { return m_userdata; }
};

template <class R, class... Ps>
Callback(const Function<R(Ps...)>&) -> Callback<R(Ps...)>;

template <size_t S, class R, class... Ps>
Callback(const StaticFunction<R(Ps...), S>&) -> Callback<R(Ps...)>;

}  // namespace upp
