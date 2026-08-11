#pragma once

namespace upp {

/**
 * \brief Utility for marking variables unused
 * */
constexpr void unused([[maybe_unused]] auto&&... args) noexcept {}

}  // namespace upp
