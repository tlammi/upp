#pragma once

#include <concepts>
#include <iterator>

namespace upp {

template <class T>
concept intrusive_list_node = !std::movable<T>;

template <class T>
struct intrusive_list_traits {
    static constexpr T*& next(T* curr) noexcept { return curr->next(); }
};

template <intrusive_list_node T, class Traits = intrusive_list_traits<T>>
class intrusive_list {
    T* m_front{};
    T* m_back{};

 public:
    constexpr intrusive_list() noexcept = default;

    template <class... Ts>
    constexpr explicit intrusive_list(std::in_place_t, Ts&... ts) noexcept {
        (push_back(ts), ...);
    }

    constexpr intrusive_list(std::initializer_list<T&> il) noexcept {
        for (auto& i : il) push_back(i);
    }

    constexpr void push_back(T& t) noexcept {
        Traits::next(&t) = nullptr;
        if (!m_front) {
            m_front = &t;
            m_back = &t;
            return;
        }
        Traits::next(m_back) = &t;
        m_back = &t;
    }

    constexpr void pop_front() noexcept { m_front = Traits::next(m_front); }

    class iterator {
        T* m_ptr{};

     public:
        using value_type = T;
        using reference = T&;
        using pointer_type = T*;
        using difference_type = std::ptrdiff_t;

        constexpr explicit iterator(T* ptr) noexcept : m_ptr(ptr) {}

        constexpr auto& operator*() const noexcept { return *m_ptr; }
        constexpr auto* operator->() const noexcept { return m_ptr; }

        constexpr bool operator==(std::default_sentinel_t) const noexcept {
            return !m_ptr;
        }

        constexpr auto& operator++() noexcept {
            m_ptr = Traits::next(m_ptr);
            return *this;
        }

        constexpr auto operator++(int) noexcept {
            auto tmp = *this;
            m_ptr = Traits::next(m_ptr);
            return tmp;
        }
    };

    constexpr auto begin() noexcept { return iterator(m_front); }
    constexpr auto end() noexcept { return std::default_sentinel; }

    constexpr T& front() const noexcept { return *m_front; }
    constexpr T& back() const noexcept { return *m_back; }

    constexpr bool empty() const noexcept { return !m_front; }

    constexpr std::size_t size() const noexcept {
        auto* ptr = m_front;
        std::size_t counter = 0;
        while (ptr) {
            ++counter;
            ptr = Traits::next(ptr);
        }
        return counter;
    }

    constexpr auto length() const noexcept { return size(); }
};
}  // namespace upp
