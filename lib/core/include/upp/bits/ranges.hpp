#pragma once

#include <ranges>
namespace upp {

template <class T>
constexpr auto range(T&& v) {
    return std::views::iota(std::remove_cvref_t<T>{}, std::forward<T>(v));
}

}  // namespace upp
