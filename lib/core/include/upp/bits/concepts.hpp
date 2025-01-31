#pragma once

#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_utility.hpp>
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

namespace detail {

template <enum_type E>
constexpr bool is_enum_bitmask()
    requires(magic_enum::enum_count<E>() > 0)
{
    auto mask = std::underlying_type_t<E>{};
    bool success = true;
    magic_enum::enum_for_each<E>([&](E v) {
        if (!success) return;
        auto u = static_cast<std::underlying_type_t<E>>(v);
        if (u & mask) success = false;
        mask |= u;
    });
    return success;
}

template <concepts::enum_type E>
constexpr bool is_enum_bitmask()
    requires(magic_enum::enum_count<E>() == 0)
{
    return true;
}

}  // namespace detail

template <class E>
concept enum_bitmask = enum_type<E> && detail::is_enum_bitmask<E>();

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
