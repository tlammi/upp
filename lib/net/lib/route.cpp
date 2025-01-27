#include <cassert>
#include <ranges>
#include <stdexcept>
#include <upp/bits/net/route.hpp>
#include <upp/str.hpp>

namespace upp::net {
namespace rv = std::ranges::views;
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
namespace route_detail {

std::optional<std::map<std::string_view, std::string_view>> parse_route_params(
    std::string_view pattern, std::string_view path) {
    auto patt_view = pattern | rv::split('/');
    auto path_view = path | rv::split('/');
    auto patt_iter = patt_view.begin();
    auto path_iter = path_view.begin();
    std::map<std::string_view, std::string_view> map{};
    while (patt_iter != patt_view.end() || path_iter != path_view.end()) {
        if (patt_iter == patt_view.end() || path_iter == path_view.end())
            return std::nullopt;

        auto patt_view =
            std::string_view((*patt_iter).data(), (*patt_iter).size());
        auto path_view =
            std::string_view((*path_iter).data(), (*path_iter).size());

        if (patt_view.starts_with('{') && patt_view.ends_with('}')) {
            patt_view = patt_view.substr(1, patt_view.size() - 2);
            map[patt_view] = path_view;
        } else {
            if (patt_view != path_view) return std::nullopt;
        }
        ++patt_iter;
        ++path_iter;
    }
    return map;
}

}  // namespace route_detail

}  // namespace upp::net
