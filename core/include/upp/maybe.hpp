#pragma once

#include <span>
#include <string_view>
#include <type_traits>
#include <utility>

namespace upp {
template <class T>
struct maybe_traits;

template <class T>
struct maybe_traits<T*> {
    static constexpr bool is_null(T* val) noexcept { return val == nullptr; }
    static constexpr T* null_value() noexcept { return nullptr; }
};

template <class T, std::size_t Extent>
struct maybe_traits<std::span<T, Extent>> {
    static constexpr bool is_null(std::span<const T, Extent> val) noexcept {
        return val.data() == nullptr;
    }
    static constexpr std::span<T, Extent> null_value() noexcept { return {}; }
};

template <class Char, class Traits>
struct maybe_traits<std::basic_string_view<Char, Traits>> {
    static constexpr bool is_null(
        std::basic_string_view<Char, Traits> val) noexcept {
        return val.data() == nullptr;
    }

    static constexpr std::basic_string_view<Char, Traits>
    null_value() noexcept {
        return {};
    }
};

template <std::unsigned_integral I>
struct maybe_traits<I> {
    static constexpr bool is_null(I val) noexcept {
        return val == null_value();
    }

    static constexpr I null_value() noexcept {
        return std::numeric_limits<I>::max();
    }
};

struct null_maybe_t {};

constexpr null_maybe_t null_maybe{};

/**
 * \brief std::optional-like type without an extra bool
 *
 * This type works as std::optional but uses special null values
 * of the stored value to check for validity
 * */
template <class T, class Traits = maybe_traits<T>>
class maybe {
 private:
    T m_val{Traits::null_value()};

 public:
    constexpr maybe() noexcept(
        std::is_nothrow_constructible_v<T, decltype(Traits::null_value())>) =
        default;

    constexpr maybe(null_maybe_t) noexcept(
        std::is_nothrow_constructible_v<T, decltype(Traits::null_value())>) {}

    template <class... Ts>
    constexpr explicit maybe(std::in_place_t, Ts&&... ts) noexcept(
        std::is_nothrow_constructible_v<T, Ts...>)
        : m_val(std::forward<Ts>(ts)...) {}

    constexpr explicit maybe(const T& t) noexcept(
        std::is_nothrow_copy_constructible_v<T>)
        : m_val(t) {}

    constexpr explicit maybe(T&& t) noexcept(
        std::is_nothrow_move_constructible_v<T>)
        : m_val(std::move(t)) {}

    constexpr explicit operator bool() const noexcept { return has_value(); }

    constexpr bool has_value() const noexcept {
        return !Traits::is_null(m_val);
    }

    template <class S>
    constexpr decltype(auto) operator*(this S&& self) noexcept {
        return std::forward<S>(self).m_val;
    }

    template <class S>
    constexpr T* operator->(this S& self) noexcept {
        return &self.m_val;
    }
};
}  // namespace upp
