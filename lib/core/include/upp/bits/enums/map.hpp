#pragma once

#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <upp/bits/enums/detail/map_iterator.hpp>
#include <upp/concepts.hpp>

namespace upp {

template <concepts::enum_type Key, class Val, class Store>
class EnumMapBase {
    Store m_data{};
    size_t m_size{};

 public:
    using key_type = Key;
    using mapped_type = Val;
    using value_type = std::pair<key_type, mapped_type>;
    using iterator = detail::MapIterator<Key, Val, Store, false>;
    using const_iterator = detail::MapIterator<Key, Val, Store, true>;

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

    iterator end() noexcept { return {&m_data}; }
    const_iterator end() const noexcept { return {&m_data}; }

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
