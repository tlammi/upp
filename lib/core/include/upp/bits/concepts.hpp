#pragma once

#include <type_traits>

namespace upp::concepts {
namespace detail {

template <class T, class Proto, class = void>
struct prototype_impl : std::false_type {};

template <class T, class R, class... Ps>
struct prototype_impl<
    T, R(Ps...),
    std::enable_if_t<std::is_same_v<std::invoke_result_t<T, Ps...>, R>>>
    : std::true_type {};

}  // namespace detail
template <class T>
concept enum_type = std::is_enum_v<T>;

template <class T, class F>
concept prototype = detail::prototype_impl<T, F>::value;

}  // namespace upp::concepts
