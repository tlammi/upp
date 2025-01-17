#pragma once

#include <upp/concepts.hpp>
#include <upp/enum.hpp>

namespace upp {

template <class T, enum_type E>
class enum_array {
    static_assert(std::unsigned_integral<std::underlying_type_t<E>>,
                  "Unsigned underlying type required for enum");
    std::array<T, enum_count<E>()> m_arr;

 public:
    constexpr enum_array() noexcept = default;

    template <class... Ts>
        requires((std::convertible_to<Ts, T> && ...))
    constexpr enum_array(Ts&&... ts) : m_arr{std::forward<Ts>(ts)...} {}

    template <class S>
    constexpr decltype(auto) operator[](this S&& self, E e) noexcept {
        return std::forward<S>(self).m_arr[*enum_index(e)];
    }

    constexpr auto size() const noexcept { return m_arr.size(); }
    constexpr auto length() const noexcept { return m_arr.length(); }

    constexpr auto begin() const noexcept { return m_arr.begin(); }
    constexpr auto end() const noexcept { return m_arr.end(); }

    constexpr auto cbegin() const noexcept { return m_arr.cbegin(); }
    constexpr auto cend() const noexcept { return m_arr.cend(); }

    constexpr auto rbegin() const noexcept { return m_arr.rbegin(); }
    constexpr auto rend() const noexcept { return m_arr.rend(); }

    constexpr auto crbegin() const noexcept { return m_arr.crbegin(); }
    constexpr auto crend() const noexcept { return m_arr.crend(); }
};

template <class T, enum_type E>
constexpr auto make_enum_array(std::initializer_list<std::pair<E, T>> il) {
    auto out = enum_array<T, E>();
    for (auto& i : il) { out[i.first] = std::move(i.second); }
    return out;
}

}  // namespace upp
