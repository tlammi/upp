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

template <class T>
concept basic_lockable = requires(T t) {
    t.lock();
    t.unlock();
};

template <class T>
concept lockable = requires(T t) { t.try_lock(); } && basic_lockable<T>;

template <class T>
concept shared_lockable = requires(T t) {
    t.lock_shared();
    t.try_lock_shared();
    t.unlock_shared();
};

}  // namespace upp::concepts
