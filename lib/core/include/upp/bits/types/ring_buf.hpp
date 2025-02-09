#pragma once

#include <cstddef>
#include <memory>
#include <ranges>

namespace upp {
template <class>
class RingBuf;

namespace ring_buf_detail {

template <class T, bool Const>
struct add_const {
    using type = T;
};

template <class T>
struct add_const<T, true> {
    using type = const T;
};

template <class T, bool B>
using add_const_t = typename add_const<T, B>::type;

template <class T, bool Const>
class Iterator {
    using size_type = RingBuf<T>::size_type;
    using Store = add_const_t<typename RingBuf<T>::Store, Const>;
    template <class, bool>
    friend class Iterator;

    Store* m_store{};
    size_type m_idx{std::numeric_limits<size_type>::max()};
    friend class RingBuf<T>;

    constexpr Iterator(Store* store, size_type idx)
        : m_store(store), m_idx(idx) {}

 public:
    using value_type = add_const_t<T, Const>;
    using reference = value_type&;
    using pointer_type = value_type*;
    using difference_type = std::ptrdiff_t;

    constexpr Iterator() = default;

    constexpr Iterator(const Iterator&) noexcept = default;
    constexpr Iterator& operator=(const Iterator&) noexcept = default;

    constexpr Iterator(Iterator&&) noexcept = default;
    constexpr Iterator& operator=(Iterator&&) noexcept = default;

    constexpr ~Iterator() = default;

    template <class U, bool B>
    constexpr bool operator==(const Iterator<U, B>& other) const noexcept {
        return m_idx == other.m_idx;
    }

    template <class U, bool B>
    constexpr auto operator<=>(const Iterator<U, B>& other) const noexcept {
        return m_idx <=> other.m_idx;
    }

    constexpr Iterator& operator+=(difference_type t) noexcept {
        m_idx += t;
        return *this;
    }

    constexpr Iterator operator+(difference_type t) const noexcept {
        auto cpy = *this;
        cpy.m_idx += t;
        return cpy;
    }

    template <class U, bool B>
    constexpr difference_type operator-(
        const Iterator<U, B>& other) const noexcept {
        return m_idx - other.m_idx;
    }

    constexpr Iterator& operator-=(difference_type t) noexcept {
        m_idx -= t;
        return *this;
    }
    constexpr Iterator operator-(difference_type t) const noexcept {
        auto cpy = *this;
        cpy.m_idx -= t;
        return cpy;
    }

    constexpr Iterator& operator++() noexcept {
        ++m_idx;
        return *this;
    }

    constexpr Iterator operator++(int) noexcept {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    constexpr Iterator& operator--() noexcept {
        --m_idx;
        return *this;
    }

    constexpr Iterator operator--(int) noexcept {
        auto tmp = *this;
        --*this;
        return tmp;
    }

    constexpr reference operator*() const noexcept { return (*this)[m_idx]; }

    constexpr pointer_type operator->() const noexcept { return &*(*this); }

    constexpr reference operator[](difference_type t) const noexcept {
        return m_store->data[(m_store->offset + t) % m_store->cap].val;
    }
};

template <class T, bool B>
constexpr Iterator<T, B> operator+(typename Iterator<T, B>::difference_type a,
                                   const Iterator<T, B>& i) noexcept {
    return i + a;
}

}  // namespace ring_buf_detail

/**
 * \brief Resizeable ring buffer
 * */
template <class T>
class RingBuf {
    template <class T2, bool B>
    friend class ring_buf_detail::Iterator;

 public:
    using size_type = std::size_t;
    using value_type = T;

 private:
    // NOLINTNEXTLINE
    union Union {
        std::uint8_t dummy{};
        T val;
        ~Union() {}
    };

    struct Store {
        size_type cap{};
        size_type offset{};
        size_type count{};
        Union* data{};  // NOLINT
        constexpr explicit Store(size_type capacity)
            : cap(capacity), data(new Union[capacity]) {}
        Store(const Store&) = delete;
        Store& operator=(const Store&) = delete;
        Store(Store&&) = delete;
        Store& operator=(Store&&) = delete;
        constexpr ~Store() {
            for (size_t i = 0; i < count; ++i) {
                const auto idx = (offset + i) % cap;
                data[idx].val.~T();  // NOLINT
            }
            delete[] data;
        }
    };

    std::unique_ptr<Store> m_store{};

    void ensure_capacity(size_type size) {
        if (!m_store) {
            size_t cap = 1;
            while (cap < size) cap *= 2;
            m_store = std::make_unique<Store>(cap);
            return;
        }
        auto cap = m_store->cap;
        if (cap >= size) return;
        while (cap < size) cap *= 2;
        auto new_store = std::make_unique<Store>(cap);
        for (size_t i = 0; i < m_store->count; ++i) {
            const auto idx = (m_store->offset + i) % m_store->cap;
            std::construct_at(&new_store->data[i].val,
                              std::move(m_store->data[idx].val));
        }
        new_store->count = m_store->count;
        m_store = std::move(new_store);
    }

    void ensure_capacity_delta(size_t delta) {
        const auto cur = m_store ? m_store->count : 0;
        ensure_capacity(cur + delta);
    }

    template <class Iter>
    constexpr RingBuf(Iter begin, Iter end, size_type size) {
        ensure_capacity(size);
        while (begin != end) {
            emplace_back(*begin);
            ++begin;
        }
    }

 public:
    using iterator = ring_buf_detail::Iterator<T, false>;
    using const_iterator = ring_buf_detail::Iterator<T, true>;

    constexpr RingBuf() noexcept = default;

    template <class... Ts>
    constexpr explicit RingBuf(std::in_place_t /*unused*/, Ts&&... ts) {
        ensure_capacity(sizeof...(Ts));
        (emplace_back(std::forward<Ts>(ts)), ...);
    }

    template <class Iter>
    constexpr RingBuf(Iter begin, Iter end) {
        if constexpr (requires(Iter a, Iter b) { a - b; }) {
            ensure_capacity(end - begin);
        }
        while (begin != end) {
            emplace_back(*begin);
            ++begin;
        }
    }
    constexpr RingBuf(std::initializer_list<T> il)
        : RingBuf(il.begin(), il.end(), il.size()) {}

    constexpr RingBuf(const RingBuf& other)
        : RingBuf(other.begin(), other.end()) {}

    constexpr RingBuf& operator=(const RingBuf& other) {
        *this = RingBuf(other.begin(), other.end());
        return *this;
    }

    constexpr RingBuf(RingBuf&&) noexcept = default;
    constexpr RingBuf& operator=(RingBuf&&) noexcept = default;

    constexpr ~RingBuf() = default;

    iterator begin() noexcept { return {m_store.get(), 0}; }
    const_iterator begin() const noexcept {
        return const_iterator{m_store.get(), 0};
    }
    const_iterator cbegin() const noexcept { return {m_store.get(), 0}; }
    iterator end() noexcept { return {m_store.get(), m_store->count}; }
    const_iterator end() const noexcept {
        return {m_store.get(), m_store->count};
    }
    const_iterator cend() const noexcept {
        return {m_store.get(), m_store->count};
    }

    constexpr bool empty() const noexcept {
        return m_store ? m_store->count == 0 : true;
    }
    constexpr size_type size() const noexcept {
        if (!m_store) return 0;
        return m_store->count;
    }

    constexpr size_type length() const noexcept { return size(); }

    constexpr size_type capacity() const noexcept {
        if (!m_store) return 0;
        return m_store->cap;
    }

    template <std::equality_comparable_with<T> T2>
    constexpr bool operator==(const RingBuf<T2>& other) const noexcept {
        if (size() != other.size()) return false;
        for (const auto& pair : std::views::zip(*this, other)) {
            const auto& [l, r] = pair;
            if (!(l == r)) return false;
        }
        return true;
    }

    template <class T2>
    constexpr auto operator<=>(const RingBuf<T2>& other) const noexcept {
        return std::lexicographical_compare_three_way(
            begin(), end(), other.begin(), other.end());
    }

    constexpr void shrink_to_fit() {
        if (!m_store) return;
        auto new_cap = m_store->cap;
        while (new_cap > m_store->size) { new_cap /= 2; }
        new_cap *= 2;
        if (new_cap < m_store->cap) {
            auto new_store = std::unique_ptr<Store>(new_cap);
            for (size_t i = 0; i < m_store->count; ++i) {
                const auto idx = (m_store->offset + i) % m_store->cap;
                std::construct_at(&new_store->data[i].val,
                                  std::move(m_store->data[idx]));
            }
            m_store = std::move(new_store);
        }
    }

    constexpr void reserve(size_t hint) { ensure_capacity(hint); }

    void push_back(const T& t) { emplace_back(t); }
    void push_back(T&& t) { emplace_back(std::move(t)); }

    template <class... Ts>
    void emplace_back(Ts&&... ts) {
        ensure_capacity_delta(1);
        const auto idx = (m_store->offset + m_store->count) % m_store->cap;
        std::construct_at(&m_store->data[idx].val, std::forward<Ts>(ts)...);
        ++m_store->count;
    }

    void push_front(const T& t) { emplace_front(t); }
    void push_front(T&& t) { emplace_front(std::move(t)); }

    template <class... Ts>
    void emplace_front(Ts&&... ts) {
        ensure_capacity_delta(1);
        auto offset = (m_store->offset ? m_store->offset : m_store->cap) - 1;
        std::construct_at(&m_store->data[offset].val, std::forward<Ts>(ts)...);
        m_store->offset = offset;
        ++m_store->count;
    }

    void pop_back() noexcept {
        const auto idx = (m_store->offset + m_store->count - 1) % m_store->cap;
        m_store->data[idx].val.~T();
        --m_store->count;
    }

    void pop_front() {
        m_store->data[m_store->offset].val.~T();
        m_store->offset = (m_store->offset + 1) % m_store->cap;
        --m_store->count;
    }

    T& back() noexcept { return (*this)[m_store->count - 1]; }
    const T& back() const noexcept { return (*this)[m_store->count - 1]; }

    T& front() noexcept { return (*this)[0]; }
    const T& front() const noexcept { return (*this)[0]; }

    T& operator[](size_t idx) noexcept {
        idx = (m_store->offset + idx) % m_store->cap;
        return m_store->data[idx].val;
    }
    const T& operator[](size_t idx) const noexcept {
        idx = (m_store->offset + idx) % m_store->cap;
        return m_store->data[idx].val;
    }

    T& at(size_t idx) {
        if (!m_store || idx >= m_store->count)
            throw std::runtime_error("RingBuf::at");
        return (*this)[idx];
    }
    const T& at(size_t idx) const {
        if (!m_store || idx >= m_store->count)
            throw std::runtime_error("RingBuf::at");
        return (*this)[idx];
    }

    T* data() noexcept { return m_store ? m_store->data : nullptr; }
    const T* data() const noexcept { return m_store ? m_store->data : nullptr; }

    size_type offset() const noexcept { return m_store ? m_store->offset : 0; }
};

template <class T, class U>
constexpr auto operator+(
    typename RingBuf<T>::template IteratorImpl<U>::difference_type a,
    const typename RingBuf<T>::template IteratorImpl<U>& b)
    -> RingBuf<T>::template IteratorImpl<U> {
    return b + a;
}

}  // namespace upp
