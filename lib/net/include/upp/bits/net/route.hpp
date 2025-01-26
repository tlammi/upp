#pragma once

#include <map>
#include <optional>
#include <string_view>

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
std::optional<RouteView> parse_route(std::string_view pattern,
                                     std::string_view path);

}  // namespace upp::net
