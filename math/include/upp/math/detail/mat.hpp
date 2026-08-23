#pragma once

#include <cstddef>
#include <memory>
#include <ranges>
#include <upp/fam.hpp>
#include <upp/math/detail/row.hpp>
#include <upp/math/detail/traits.hpp>
#include <upp/math/detail/views.hpp>
#include <upp/type_traits.hpp>

namespace upp::math::detail {

template <class T>
struct mat_store {
    size_type h;
    size_type w;

    std::span<T> data(this auto& self) noexcept {
        return {upp::fam::get_fam<mat_store, T>(&self), self.h * self.w};
    }
};

template <class T>
struct mat_store_deleter {
    void operator()(mat_store<T>* s) {
        for (auto& i : s->data()) std::destroy_at(&i);
        std::destroy_at(s);
        upp::fam::dealloc<mat_store<T>, T>(s);
    }
};

template <class T>
using mat_store_ptr = std::unique_ptr<mat_store<T>, mat_store_deleter<T>>;

template <class T>
auto make_mat_store(size_type h, size_type w) {
    auto* data = fam::alloc<mat_store<T>, T>(h * w);
    std::construct_at(data);
    data->h = h;
    data->w = w;
    // TODO: Handle throws
    for (size_type i = 0; i < h * w; ++i) {
        std::construct_at(&data->data()[i]);
    }
    return mat_store_ptr<T>(data, {});
}

template <class T>
auto cpy_mat_store(const mat_store<T>& orig) {
    auto out = make_mat_store<T>(orig.h, orig.w);
    for (auto [l, r] : std::views::zip(out->data(), orig.data())) { l = r; }
    return out;
}

/**
 * \brief Dense matrix
 * \ingroup upp_math
 * */
template <class T>
class mat {
 public:
    using size_type = ::upp::math::detail::size_type;

    using value_type = T;

    static constexpr auto static_height = dynamic;
    static constexpr auto static_width = dynamic;

 private:
    mat_store_ptr<T> m_s{};

    explicit mat(mat_store_ptr<T> s) noexcept : m_s(std::move(s)) {}

 public:
    /**
     * \brief Construct matrix from rows
     *
     * Constructs a matrix with data from the rows passed in. The first row is
     * stored in the first matrix row etc. The matrix dimensions are
     * (<row_count>, <max_row_len>). For rows are amended with default
     * constructed values if the row length is less than the longest row length.
     * */
    template <class... Ts>
    explicit mat(Ts&&... ts)
        requires((is_row_v<Ts> && ...))
        : m_s(make_mat_store<T>(sizeof...(Ts), max_row_len(ts...))) {
        assign_rows(rows(), 0, std::forward<Ts>(ts)...);
    }

    constexpr mat(const mat& other) : m_s(cpy_mat_store(*other.m_s)) {}
    constexpr mat& operator=(const mat& other) {
        m_s = cpy_mat_store(other.m_s);
        return *this;
    }

    mat(mat&& other) noexcept = default;
    mat& operator=(mat&&) noexcept = default;

    ~mat() = default;

    /**
     * \brief Create a matrix with default initialized values
     *
     * \param h Matrix height
     * \param w Matrix width
     * */
    static mat zeros(size_type h, size_type w) {
        return mat{make_mat_store<T>(h, w)};
    }

    /**
     * \brief Create an identify matrix
     *
     * \param h Matrix height
     * \param w Matrix width
     * */
    static mat eye(size_type h, size_type w) {
        auto s = make_mat_store<T>(h, w);
        for (size_type i = 0; i < std::min(h, w); ++i) {
            s->data()[flatten_index({.h = h, .w = w, .i = i, .j = i})] = 1;
        }
        return mat{std::move(s)};
    }

    /// \brief Matrix height
    size_type height() const noexcept { return m_s->h; }

    /// \brief Matrix width
    size_type width() const noexcept { return m_s->w; }

    /**
     * \brief Matrix dimensions
     *
     * Returns a type with .h and .w members that can be unpacked with
     * structured bindings: [h, w] = m.dims()
     * */
    auto dims() const noexcept {
        struct dims_t {
            size_type h;
            size_type w;

            constexpr auto operator<=>(const dims_t&) const noexcept = default;
        };
        return dims_t{.h = height(), .w = width()};
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
        return UPP_FWD(self).m_s->data()[idx];
    }

    /**
     * \brief View providing access to matrix rows
     * */
    auto rows(this auto& self) noexcept { return rows_view(self); }

    /**
     * \brief View providing access to matrix columns.
     * */
    auto columns(this auto& self) noexcept { return columns_view(self); }
};

}  // namespace upp::math::detail
