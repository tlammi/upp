#pragma once

#include <span>
#include <upp/concepts.hpp>

namespace upp {

template <enum_type E>
constexpr auto underlying_cast(E e) noexcept {
    return static_cast<std::underlying_type_t<E>>(e);
}

template <enum_type E>
constexpr E underlying_cast(std::underlying_type_t<E> e) noexcept {
    return static_cast<E>(e);
}

template <class T, class U>
constexpr T static_or_reinterpret_cast(U&& u) noexcept(
    !std::convertible_to<U, T> || std::is_nothrow_convertible_v<U, T>) {
    if constexpr (std::convertible_to<U, T>) {
        return static_cast<T>(std::forward<U>(u));
    } else {
        // NOLINTNEXTLINE
        return reinterpret_cast<T>(std::forward<U>(u));
    }
}

template <class T, class U, std::size_t Extent>
constexpr auto span_cast(std::span<U, Extent> span) noexcept {
    if constexpr (Extent == std::dynamic_extent)
        return std::span<T>(static_or_reinterpret_cast<T*>(span.data()),
                            span.size());
    else
        return std::span<T>(static_or_reinterpret_cast<T*>(span.data()));
}

}  // namespace upp
