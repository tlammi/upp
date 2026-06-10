#pragma once

#include <ranges>
#include <upp/concepts.hpp>
#include <upp/detail/feature.hpp>

#if UPP_HAVE_REFLECTION
#include <meta>
#include <upp/concepts.hpp>
namespace upp {

template <enum_type E>
consteval auto enum_count() noexcept {
    return std::meta::enumerators_of(^^E).size();
}

template <enum_type E>
consteval auto enum_values() noexcept {
    using namespace std::meta;
    auto out = std::array<E, enum_count<E>()>{};
    size_t i = 0;
    template for (constexpr auto inf :
                  std::define_static_array(std::meta::enumerators_of(^^E))) {
        out[i] = [:inf:];
        ++i;
    }
    return out;
}

template <enum_type E>
constexpr std::optional<std::size_t> enum_index(E e) noexcept {
    const auto vals = enum_values<E>();
    for (auto [i, v] : std::views::enumerate(vals)) {
        if (e == v) return i;
    }
    return std::nullopt;
}

}  // namespace upp
#else

#include <upp/thirdparty/magic_enum.hpp>

namespace upp {

template <enum_type E>
consteval auto enum_count() noexcept {
    return magic_enum::enum_count<E>();
}

template <enum_type E>
consteval auto enum_values() noexcept {
    return magic_enum::enum_values<E>();
}

template <enum_type E>
constexpr auto enum_index(E e) noexcept {
    return magic_enum::enum_index(e);
}

}  // namespace upp
#endif
