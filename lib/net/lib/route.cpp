#include <cassert>
#include <print>
#include <stdexcept>
#include <upp/bits/net/route.hpp>
#include <upp/str.hpp>

namespace upp::net {
namespace {
std::string_view split_next(std::string_view& s) {
    if (s.starts_with('{')) {
        auto idx = s.find('}');
        if (idx == std::string_view::npos)
            throw std::runtime_error("invalid pattern");
        auto res = s.substr(0, idx + 1);
        s.remove_prefix(idx + 1);
        return res;
    }
    auto idx = s.find('{');
    if (idx == std::string_view::npos) return std::exchange(s, {});
    auto res = s.substr(0, idx);
    s.remove_prefix(idx);
    return res;
}
}  // namespace

std::optional<RouteView> parse_route(std::string_view pattern,
                                     std::string_view path) {
    namespace rv = std::ranges::views;
    auto orig = path;
    RouteView::ParamMap map{};
    std::string_view key{};
    while (!pattern.empty()) {
        auto val = split_next(pattern);
        if (val.empty()) break;
        std::println("val: '{}'", val);
        if (val.starts_with('{') && val.ends_with('}')) {
            key = val.substr(1, val.size() - 2);
            if (key.empty()) throw std::runtime_error("invalid pattern");
        } else {
            if (key.empty()) {
                if (!path.starts_with(val)) { return std::nullopt; }
                std::println("before discard: {}", path);
                path.remove_prefix(val.size());
                std::println("after discard: {}", path);
            } else {
                auto idx = path.find(val);
                if (idx == std::string_view::npos) return std::nullopt;
                std::println("assigning to map: '{}': '{}'", key,
                             path.substr(0, idx));
                map[key] = path.substr(0, idx);
                std::println("adsf: {}, {}", key, map[key]);
                std::println("a: {}", map["a"]);
                std::println("b: {}", map["b"]);
                path.remove_prefix(idx + val.size());
                std::println("path left: {}", path);
                key = {};
            }
        }
    }
    if (!key.empty()) {
        map[key] = path;
        std::println("last: {}: {}", key, map[key]);
    }
    std::println("map:");
    for (const auto& [k, v] : map) { std::println("{}: {}", k, v); }

    return RouteView(orig, std::move(map));
}
}  // namespace upp::net
