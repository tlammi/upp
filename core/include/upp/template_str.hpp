#pragma once

#include <array>
#include <cstddef>
#include <string_view>

namespace upp {

/**
 * \brief Type for passing strings as template arguments
 * */
template <std::size_t N>
struct template_str {
    std::array<char, N> data{};

    consteval template_str(const char (&s)[N] /*NOLINT*/) {
        std::copy(s, s + N, data.begin());
    }

    constexpr std::string_view view() const noexcept {
        return {data.data(), data.size() - 1};
    }
};

template <std::size_t N>
template_str(const char (&)[N] /*NOLINT*/) -> template_str<N>;

}  // namespace upp
