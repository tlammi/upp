#pragma once

#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <upp/concepts.hpp>

namespace upp {

template <concepts::enum_type Key, class Val>
class EnumMap {
  using Entry = std::optional<Val>;
  std::unique_ptr<Entry[]> m_data{new Entry[map_capacity]};
  size_t m_size{};

 public:
  using key_type = Key;
  using mapped_type = Val;
  using value_type = std::pair<key_type, mapped_type>;

  static constexpr size_t map_capacity = magic_enum::enum_count<Key>();

  constexpr EnumMap() = default;
  constexpr EnumMap(std::initializer_list<value_type> list) {
    for (auto&& pair : list) {
      operator[](pair.first) = std::move(pair.second);
    }
  }

  constexpr size_t capacity() const noexcept { return map_capacity; }
  constexpr size_t size() const noexcept { return m_size; }
  [[nodiscard]] constexpr bool empty() const noexcept { return !m_size; }

  mapped_type& operator[](Key key) {
    auto& elem = m_data[underlying_cast(key)];
    if (!elem) {
      elem.emplace();
      ++m_size;
    }
    elem.emplace();
    return *elem;
  }

  mapped_type& at(Key key) {
    auto& elem = m_data[underlying_cast(key)];
    return elem.value();
  }

  const mapped_type& at(Key key) const {
    auto& elem = m_data[underlying_cast(key)];
    return elem.value();
  }
};

}  // namespace upp
