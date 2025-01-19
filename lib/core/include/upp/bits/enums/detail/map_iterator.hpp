#pragma once

#include <cstddef>
#include <magic_enum/magic_enum.hpp>
#include <upp/concepts.hpp>
#include <utility>

namespace upp {
template <concepts::enum_type, class, class>
class EnumMapBase;

namespace detail {

template <concepts::enum_type Key, class Val, class Store, bool Const>
class MapIterator {
    template <concepts::enum_type, class, class>
    friend class ::upp::EnumMapBase;

 public:
    using difference_type = typename std::ptrdiff_t;
    using reference = std::conditional_t<Const, const Val&, Val&>;
    using store_pointer = std::conditional_t<Const, const Store*, Store*>;
    using value_type = std::pair<Key, Val>;

 private:
    store_pointer m_store{};
    union {
        Key key{};
        std::pair<Key, reference> pair;
    } m_data{};
    bool m_end{false};

    constexpr bool is_end() const noexcept { return m_end; }

    constexpr size_t idx() noexcept {
        return *magic_enum::enum_index(m_data.key);
    }

    constexpr void inc() noexcept {
        auto i = idx() + 1;
        if (i == magic_enum::enum_count<Key>()) {
            m_end = true;
        } else {
            m_data.key = magic_enum::enum_value<Key>(i);
        }
    }

    constexpr void dec() noexcept {
        if (m_end) {
            auto count = magic_enum::enum_count<Key>();
            m_data.key = magic_enum::enum_value<Key>(count - 1);
            m_end = false;
        } else {
            auto i = idx();
            assert(i);
            m_data.key = magic_enum::enum_value<Key>(i - 1);
        }
    }

    constexpr MapIterator(Key key, store_pointer store) noexcept
        : m_store(store), m_data{.key = key} {
        assert(&m_data.key == &m_data.pair.first);
        if (is_end()) return;
        while (true) {
            auto& optional = (*m_store)[idx()];
            if (optional) {
                std::construct_at(&m_data.pair, m_data.pair.first, *optional);
                return;
            }
            inc();
            if (is_end()) return;
        }
    }

    constexpr MapIterator(store_pointer store) noexcept
        : m_store(store), m_end(true) {}

 public:
    constexpr MapIterator() noexcept = default;

    constexpr MapIterator(const MapIterator& other) noexcept
        : m_store(other.m_store), m_end(other.m_end) {
        std::construct_at(&m_data.pair, other.m_data.pair);
    }

    constexpr MapIterator& operator=(const MapIterator& other) noexcept {
        m_store = other.m_store;
        m_end = other.m_end;
        std::construct_at(&m_data.pair, other.m_data.pair);
        return *this;
    }

    constexpr MapIterator(MapIterator&& other) noexcept
        : m_store(other.m_store), m_end(other.m_end) {
        std::construct_at(&m_data.pair, other.pair);
    }

    constexpr MapIterator& operator=(MapIterator&& other) noexcept {
        m_store = other.m_store;
        m_end = other.m_end;
        std::construct_at(&m_data.pair, other.pair);
        return *this;
    }

    constexpr ~MapIterator() = default;

    constexpr bool operator==(const MapIterator& other) const noexcept {
        if (m_end && other.m_end) return true;
        if (m_end || other.m_end) return false;
        return m_data.key == other.m_data.key;
    }

    constexpr MapIterator& operator++() noexcept {
        while (true) {
            inc();
            if (is_end()) return *this;
            auto& optional = (*m_store)[idx()];
            if (optional) {
                std::construct_at(&m_data.pair, m_data.pair.first, *optional);
                return *this;
            }
        }
    }

    constexpr MapIterator operator++(int) noexcept {
        auto cpy = *this;
        ++(*this);
        return cpy;
    }

    constexpr MapIterator& operator--() noexcept {
        while (true) {
            dec();
            auto& optional = (*m_store)[idx()];
            if (optional) {
                std::construct_at(&m_data.pair, m_data.pair.first, *optional);
                return *this;
            }
        }
    }

    constexpr MapIterator operator--(int) noexcept {
        auto cpy = *this;
        --(*this);
        return cpy;
    }

    template <class S>
    constexpr decltype(auto) operator*(this S&& self) noexcept {
        return std::forward<S>(self).m_data.pair;
    }

    constexpr std::pair<Key, reference>* operator->() noexcept {
        return &m_data.pair;
    }

    constexpr const std::pair<Key, reference>* operator->() const noexcept {
        return &m_data.pair;
    }
};
}  // namespace detail
}  // namespace upp
