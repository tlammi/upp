#pragma once

#include <utility>
#include <variant>

namespace upp {
namespace match_detail {
template <class... Ts>
struct Visitor final : public Ts... {
  constexpr Visitor(Ts... ts) : Ts(std::forward<Ts>(ts))... {}
  using Ts::operator()...;
};

template <class... Ts>
Visitor(Ts...) -> Visitor<Ts...>;
}  // namespace match_detail

template <class Variant, class... Ts>
constexpr decltype(auto) match(Variant&& var, Ts&&... ts) {
  return std::visit(match_detail::Visitor(std::forward<Ts>(ts)...),
                    std::forward<Variant>(var));
}

}  // namespace upp
