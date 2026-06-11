#pragma once

#include <upp/detail/feature.hpp>
#if UPP_HAVE_REFLECTION
#include <meta>
#endif
#include <upp/cast.hpp>
#include <upp/enum_array.hpp>

namespace upp::terminal {

enum class option {
    clear = 0,
    bold,
    faint,
    italic,
    underline,
    slow_blink,

    black = 30,
    red,
    green,
    yellow,
    blue,
    purple,
    cyan,
    white,

    bg_black = 40,
    bg_red,
    bg_green,
    bg_yellow,
    bg_blue,
    bg_purple,
    bg_cyan,
    bg_white,
};

template <class... Ts>
constexpr auto dynamic_escape_code(Ts&&... ts) {
    std::string res{"\033["};

    if constexpr (sizeof...(Ts) > 0) {
        auto append = [&](auto v) {
            res += std::to_string(underlying_cast(v));
            res.push_back(';');
        };
        (append(std::forward<Ts>(ts)), ...);
        res.pop_back();
    }
    res.push_back('m');
    return res;
}
#if UPP_HAVE_REFLECTION
template <class... Ts>
consteval auto escape_code(Ts&&... ts) {
    auto s = dynamic_escape_code(std::forward<Ts>(ts)...);
    return std::define_static_string(s);
}
#endif
}  // namespace upp::terminal
