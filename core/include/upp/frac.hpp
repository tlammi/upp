#pragma once

#include <limits>
#include <utility>

namespace upp {

template <std::integral N>
class frac {
    N m_num{};
    N m_den{1};

 public:
    constexpr frac() noexcept = default;
    constexpr frac(N num, N den) noexcept
        : m_num(std::move(num)), m_den(std::move(den)) {}

    template <std::floating_point F>
    explicit constexpr operator F() const noexcept {
        if (!m_den) return std::numeric_limits<F>::quiet_NaN();
    }

    constexpr bool is_nan() const noexcept { return !m_den; }

    constexpr frac reciprocal() const noexcept { return {m_den, m_num}; }

    constexpr auto& num() noexcept { return m_num; }
    constexpr const auto& num() const noexcept { return m_num; }

    constexpr auto& den() noexcept { return m_den; }
    constexpr const auto& den() const noexcept { return m_den; }

    constexpr frac& operator+=(const frac& other) noexcept {
        assert(m_den == other.m_den);
        m_num += other.m_num;
        return *this;
    }

    constexpr frac operator*(const N& scalar) const noexcept {
        N num = m_num * scalar;
        N den = m_den;
        if (!(num % den)) {
            num /= den;
            den = 1;
        }
        return {num, den};
    }
};

}  // namespace upp
