#pragma once

#include <array>
#include <cassert>
#include <upp/forward.hpp>
#include <upp/math/detail/row.hpp>
#include <upp/math/detail/traits.hpp>

namespace upp::math::detail {

template <class T, size_type H, size_type W>
class static_mat {
 public:
    static constexpr auto element_count = H * W;
    static constexpr auto static_height = H;
    static constexpr auto static_width = W;
    using size_type = upp::math::detail::size_type;

    using value_type = T;

 private:
    using array_type = std::array<T, element_count>;
    array_type m_arr{};

 public:
    constexpr static_mat() noexcept = default;

    template <class... Ts>
    constexpr explicit static_mat(Ts&&... ts)
        requires((is_row_v<Ts> && ...))
    {
        assign_rows(rows(), 0, std::forward<Ts>(ts)...);
    }

    static constexpr static_mat zeros() { return {}; }

    static constexpr static_mat eye() {
        auto self = static_mat();
        for (auto i = size_type{}; i < std::min(H, W); ++i) { self[i, i] = 1; }
        return self;
    }

    /**
     * \brief Access matrix data
     *
     * \param i Row index (starting from 0)
     * \param j Column index (starting from 0)
     * */
    decltype(auto) operator[](this auto&& self, size_type i, size_type j) {
        assert(i < self.height());
        assert(j < self.width());
        auto idx = flatten_index(
            {.h = self.height(), .w = self.width(), .i = i, .j = j});
        return UPP_FWD(self).m_arr[idx];
    }

    /**
     * \brief View providing access to matrix rows
     * */
    auto rows(this auto& self) noexcept { return rows_view(self); }

    /**
     * \brief View providing access to matrix columns.
     * */
    auto columns(this auto& self) noexcept { return columns_view(self); }

    constexpr auto height() const noexcept { return H; }
    constexpr auto width() const noexcept { return W; }
};

template <class T, class... Ts>
constexpr auto make_static_mat(Ts&&... ts)
    requires((is_row_v<Ts> && ...))
{
    static constexpr auto height = sizeof...(Ts);
    static constexpr auto width = max_row_len(ts...);
    return static_mat<T, height, width>(UPP_FWD(ts)...);
}

}  // namespace upp::math::detail
