#pragma once

#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <upp/concepts.hpp>

namespace upp {

template <concepts::enum_type Key, class Val, class Store>
class EnumMapBase {
    Store m_data{};
    size_t m_size{};

 public:
    template <bool Const>
    class IteratorImpl {
        friend class EnumMapBase;

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

        constexpr bool is_end() const noexcept { return !m_store; }

        constexpr size_t idx() noexcept {
            return *magic_enum::enum_index(m_data.key);
        }

        constexpr void inc() noexcept {
            auto i = idx() + 1;
            if (i == magic_enum::enum_count<Key>()) {
                m_store = nullptr;
            } else {
                m_data.key = magic_enum::enum_value<Key>(i);
            }
        }

        constexpr IteratorImpl(Key key, store_pointer store) noexcept
            : m_store(store), m_data{.key = key} {
            assert(&m_data.key == &m_data.pair.first);
            if (is_end()) return;
            while (true) {
                auto& optional = (*m_store)[idx()];
                if (optional) {
                    std::construct_at(&m_data.pair, m_data.pair.first,
                                      *optional);
                    return;
                }
                inc();
                if (is_end()) return;
            }
        }

     public:
        constexpr IteratorImpl() noexcept = default;

        constexpr IteratorImpl(const IteratorImpl& other) noexcept
            : m_store(other.m_store) {
            if (m_store) std::construct_at(&m_data.pair, other.m_data.pair);
        }

        constexpr IteratorImpl& operator=(const IteratorImpl& other) noexcept {
            m_store = other.m_store;
            std::construct_at(&m_data.pair, other.m_data.pair);
            return *this;
        }

        constexpr IteratorImpl(IteratorImpl&& other) noexcept
            : m_store(other.m_store) {
            std::construct_at(&m_data.pair, other.pair);
        }

        constexpr IteratorImpl& operator=(IteratorImpl&& other) noexcept {
            m_store = other.m_store;
            std::construct_at(&m_data.pair, other.pair);
            return *this;
        }

        constexpr ~IteratorImpl() = default;

        constexpr bool operator==(const IteratorImpl& other) const noexcept {
            if (!m_store && !other.m_store) return true;
            if (m_store && other.m_store) {
                return m_data.key == other.m_data.key;
            }
            return false;
        }

        constexpr IteratorImpl& operator++() noexcept {
            while (true) {
                inc();
                if (is_end()) return *this;
                auto& optional = (*m_store)[idx()];
                if (optional) {
                    std::construct_at(&m_data.pair, m_data.pair.first,
                                      *optional);
                    return *this;
                }
            }
        }

        constexpr IteratorImpl operator++(int) noexcept {
            auto cpy = *this;
            ++(*this);
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

    using key_type = Key;
    using mapped_type = Val;
    using value_type = std::pair<key_type, mapped_type>;
    using iterator = IteratorImpl<false>;
    using const_iterator = IteratorImpl<true>;

    static constexpr size_t map_capacity = magic_enum::enum_count<Key>();

    constexpr size_t capacity() const noexcept { return map_capacity; }
    constexpr size_t size() const noexcept { return m_size; }
    [[nodiscard]] constexpr bool empty() const noexcept { return !m_size; }

    mapped_type& operator[](Key key) {
        auto& elem = m_data[*magic_enum::enum_index(key)];
        if (!elem) {
            elem.emplace();
            ++m_size;
        }
        return *elem;
    }

    mapped_type& at(Key key) {
        auto& elem = m_data[*magic_enum::enum_index(key)];
        return elem.value();
    }

    const mapped_type& at(Key key) const {
        auto& elem = m_data[*magic_enum::enum_index(key)];
        return elem.value();
    }

    iterator begin() noexcept {
        return {magic_enum::enum_value<Key>(0), &m_data};
    }

    const_iterator begin() const noexcept {
        return {magic_enum::enum_value<Key>(0), &m_data};
    }

    iterator end() noexcept { return {}; }
    const_iterator end() const noexcept { return {}; }

    size_t erase(Key key) noexcept {
        auto& opt = m_data[*magic_enum::enum_index(key)];
        if (opt) {
            opt.reset();
            --m_size;
            return 1;
        }
        return 0;
    }

 protected:
    constexpr EnumMapBase(Store init) : m_data(std::move(init)) {}
    constexpr EnumMapBase() = default;

    constexpr EnumMapBase(Store init, std::initializer_list<value_type> list)
        : m_data(std::move(init)) {
        for (auto&& pair : list) {
            operator[](pair.first) = std::move(pair.second);
        }
    }
    constexpr EnumMapBase(std::initializer_list<value_type> list) {
        for (auto&& pair : list) {
            operator[](pair.first) = std::move(pair.second);
        }
    }

    constexpr ~EnumMapBase() = default;
};  // namespace upp

template <concepts::enum_type Key, class Val>
class EnumMap
    : public EnumMapBase<Key, Val, std::unique_ptr<std::optional<Val>[]>> {
    using Base = EnumMapBase<Key, Val, std::unique_ptr<std::optional<Val>[]>>;

 public:
    constexpr EnumMap()
        : Base(std::unique_ptr<std::optional<Val>[]>(
              new std::optional<Val>[Base::map_capacity])) {}
    constexpr EnumMap(std::initializer_list<typename Base::value_type> list)
        : Base(std::unique_ptr<std::optional<Val>[]>(
                   new std::optional<Val>[Base::map_capacity]),
               list) {}
};

template <concepts::enum_type Key, class Val>
class EnumMapStatic
    : public EnumMapBase<
          Key, Val,
          std::array<std::optional<Val>, magic_enum::enum_count<Key>()>> {
    using Base = EnumMapBase<
        Key, Val,
        std::array<std::optional<Val>, magic_enum::enum_count<Key>()>>;

 public:
    constexpr EnumMapStatic() : Base() {}
    constexpr EnumMapStatic(
        std::initializer_list<typename Base::value_type> list)
        : Base(list) {}
};

}  // namespace upp
