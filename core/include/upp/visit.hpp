#pragma once

#include <variant>

namespace upp {

template <class Variant, class... Fn>
constexpr decltype(auto) visit(Variant&& variant, Fn&&... fn) {
    struct visitor : Fn... {
        visitor(Fn... fn) : Fn(fn)... {}
        using Fn::operator()...;
    };
    return std::visit(visitor(std::forward<Fn>(fn)...),
                      std::forward<Variant>(variant));
}
}  // namespace upp
