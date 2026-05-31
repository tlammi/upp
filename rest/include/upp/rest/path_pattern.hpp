#pragma once

#include <algorithm>
#include <cassert>
#include <ranges>
#include <upp/template_str.hpp>

namespace upp::rest {
struct pattern_elem {
    std::string_view key;
    std::string_view value;
};
namespace detail {

constexpr auto split(std::string_view s) noexcept {
    return s | std::views::split('/') | std::views::transform([](auto p) {
               return std::string_view(p.data(), p.size());
           });
}

template <template_str Pattern>
consteval bool is_valid_pattern() noexcept {
    auto s = Pattern.view();
    for (auto part : split(s)) {
        if (part.starts_with('{') && part.ends_with('}') && part.size() > 2)
            continue;
        if (part.contains('{') || part.contains('}')) return false;
    }
    return true;
}

consteval std::size_t field_count(std::string_view s) noexcept {
    return std::ranges::count(s, '{');
}

}  // namespace detail

template <template_str Pattern>
struct path_pattern {};

template <template_str Pattern>
constexpr auto parse_path(path_pattern<Pattern> pattern, std::string_view path)
    -> std::optional<
        std::array<pattern_elem, detail::field_count(Pattern.view())>> {
    auto out = std::array<pattern_elem, detail::field_count(Pattern.view())>{};
    auto pattern_str = Pattern.view();
    std::size_t idx = 0;
    for (auto [pattern_part, path_part] :
         std::views::zip(detail::split(pattern_str), detail::split(path))) {
        if (pattern_part.starts_with('{')) {
            auto key = pattern_part.substr(1, pattern_part.size() - 2);
            out[idx] = {key, path_part};
            ++idx;
        } else {
            if (pattern_part != path_part) return std::nullopt;
        }
    }
    return out;
}

}  // namespace upp::rest
