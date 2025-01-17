#pragma once

#include <upp/concepts.hpp>

namespace upp {

template <concepts::enum_type Enum>
constexpr std::underlying_type_t<Enum> underlying_cast(Enum e) {
  return static_cast<std::underlying_type_t<Enum>>(e);
}

template <concepts::enum_type Enum>
constexpr Enum underlying_cast(std::underlying_type_t<Enum> e) {
  return static_cast<Enum>(e);
}

}  // namespace upp
