#include <cassert>
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
    auto orig = path;
    RouteView::ParamMap map{};
    std::string_view key{};
    while (!pattern.empty()) {
        auto val = split_next(pattern);
        if (val.empty()) break;
        if (val.starts_with('{') && val.ends_with('}')) {
            key = val.substr(1, val.size() - 2);
            if (key.empty()) throw std::runtime_error("invalid pattern");
        } else {
            if (key.empty()) {
                if (!path.starts_with(val)) { return std::nullopt; }
                path.remove_prefix(val.size());
            } else {
                auto idx = path.find(val);
                if (idx == std::string_view::npos) return std::nullopt;
                map[key] = path.substr(0, idx);
                path.remove_prefix(idx + val.size());
                key = {};
            }
        }
    }
    if (!key.empty()) { map[key] = std::exchange(path, {}); }
    if (!path.empty()) { return std::nullopt; }
    return RouteView(orig, std::move(map));
}
}  // namespace upp::net
