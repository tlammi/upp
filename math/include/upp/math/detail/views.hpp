#pragma once

#include <ranges>
#include <upp/math/detail/traits.hpp>

namespace upp::math::detail {

// Wrapper to fix gtest, not really needed otherwise
template <class R>
class view_adaptor final : public std::ranges::view_interface<view_adaptor<R>> {
    R m_r;

 public:
    using value_type = std::remove_cvref_t<
        decltype(*std::declval<std::remove_cvref_t<R>>().begin())>;
    explicit constexpr view_adaptor(R r) : m_r(std::move(r)) {}
    constexpr auto begin() const noexcept { return m_r.begin(); }
    constexpr auto end() const noexcept { return m_r.end(); }
    constexpr decltype(auto) operator[](this auto&& self,
                                        size_type s) noexcept {
        return UPP_FWD(self).m_r[s];
    }
};

constexpr auto rows_view(matrixlike auto& m) noexcept {
    return view_adaptor(
        std::views::iota(std::size_t{0}, m.height()) |
        std::views::transform([&](auto row_idx) {
            return view_adaptor(
                std::views::iota(std::size_t{0}, m.width()) |
                std::views::transform(
                    [&, row_idx](std::size_t col_idx) -> decltype(auto) {
                        return m[row_idx, col_idx];
                    }));
        }));
}

constexpr auto columns_view(matrixlike auto& m) noexcept {
    return view_adaptor(
        std::views::iota(std::size_t{0}, m.width()) |
        std::views::transform([&](auto col_idx) {
            return view_adaptor(
                std::views::iota(std::size_t{0}, m.height()) |
                std::views::transform([&, col_idx](std::size_t row_idx) {
                    return m[row_idx, col_idx];
                }));
        }));
}
}  // namespace upp::math::detail
