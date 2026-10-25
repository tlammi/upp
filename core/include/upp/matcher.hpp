#pragma once

#include <tuple>
#include <upp/forward.hpp>

namespace upp {

namespace detail {

template <class T>
struct matcher_type {
    using type = T;
};

template <class T>
struct matcher_type<T&> {
    using type = std::reference_wrapper<T>;
};

template <class T>
using matcher_type_t = typename matcher_type<T>::type;

template<class Equal, class... Ts>
class matcher{
    std::tuple<matcher_type_t<Ts>...> m_tpl{};
public:
    constexpr explicit matcher(Ts... ts) : m_tpl{UPP_FWD(ts)...} {}

    template <class T>
    friend constexpr bool operator==(const T& lhs,
                                     const matcher& m) noexcept {
        return std::apply(
            [&](const auto&... ps) { return Equal{}(lhs, ps...); },
            m.m_tpl);
    }
};

template<class... Ts>
constexpr auto any_of(Ts&&... ts) {
  auto equals = [](const auto& lhs, const auto&... ps) noexcept {
    return ((lhs == ps) || ...);
  };
  return matcher<decltype(equals), Ts...>(UPP_FWD(ts)...);
}

template<class... Ts>
constexpr auto all_of(Ts&&... ts) {
  auto equals = [](const auto& lhs, const auto&... ps) noexcept {
    return ((lhs == ps) && ...);
  };
  return matcher<decltype(equals), Ts...>(UPP_FWD(ts)...);
}

template<class... Ts>
constexpr auto none_of(Ts&&... ts) {
  auto equals = [](const auto& lhs, const auto&... ps) noexcept {
    return ((lhs != ps) && ...);
  };
  return matcher<decltype(equals), Ts...>(UPP_FWD(ts)...);
}

}  // namespace detail
using detail::any_of;
using detail::all_of;
using detail::none_of;
}  // namespace upp
