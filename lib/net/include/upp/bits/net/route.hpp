#pragma once

#include <map>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace upp::net {

template <class Str>
class BasicRoute {
 public:
    using ParamMap = std::map<Str, Str>;

 private:
    ParamMap m_params{};
    Str m_path;

 public:
    constexpr BasicRoute() noexcept = default;
    BasicRoute(Str path, ParamMap params) noexcept
        : m_params(std::move(params)), m_path(path) {}

    constexpr const ParamMap& params() const noexcept { return m_params; }
};

using Route = BasicRoute<std::string>;
using RouteView = BasicRoute<std::string_view>;

namespace route_detail {
std::optional<std::map<std::string_view, std::string_view>> parse_route_params(
    std::string_view pattern, std::string_view path);
}  // namespace route_detail

constexpr bool valid_route_pattern(std::string_view pattern) {
    bool in_braces = false;
    for (char c : pattern) {
        if (c == '{') {
            bool was = std::exchange(in_braces, true);
            if (was) return false;
        } else if (c == '}') {
            bool was = std::exchange(in_braces, false);
            if (!was) return false;
        }
    }
    return true;
}

template <class Str>
class BasicRoutePattern {
    Str m_str;

 public:
    constexpr BasicRoutePattern(Str s) : m_str(std::move(s)) {
        if (!valid_route_pattern(m_str)) {
            throw std::runtime_error("invalid route pattern");
        }
    }

    template <class Str2>
    constexpr BasicRoutePattern(BasicRoutePattern<Str2> other)
        requires(!std::same_as<Str2, Str> && std::convertible_to<Str2, Str>)
        : m_str(std::move(other.m_str)) {}

    std::optional<RouteView> parse(std::string_view path) {
        auto res = route_detail::parse_route_params(m_str, path);
        if (!res) return std::nullopt;
        return RouteView(path, std::move(*res));
    }
};

using RoutePattern = BasicRoutePattern<std::string>;
using RoutePatternView = BasicRoutePattern<std::string_view>;

std::optional<RouteView> parse_route(std::string_view pattern,
                                     std::string_view path) {
    return RoutePatternView(pattern).parse(path);
}

}  // namespace upp::net
