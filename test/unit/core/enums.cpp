
#include <gtest/gtest.h>

#include <upp/enums.hpp>

enum class Int : int {
  A = 0,
  B = 1,
};

TEST(Cast, ToUnderlying) {
  ASSERT_EQ(upp::underlying_cast(Int::A), 0);
  ASSERT_EQ(upp::underlying_cast(Int::B), 1);
}

TEST(Cast, FromUnderlying) {
  ASSERT_EQ(upp::underlying_cast<Int>(0), Int::A);
  ASSERT_EQ(upp::underlying_cast<Int>(1), Int::B);
}

TEST(Map, DefaultCtor) {
  auto map = upp::EnumMap<Int, int>();
  ASSERT_EQ(map.capacity(), 2);
  ASSERT_EQ(map.size(), 0);
}

TEST(Map, InitializerList) {
  auto map = upp::EnumMap<Int, int>{{Int::A, 10}, {Int::B, 20}};
}

TEST(Map, Add) {
  auto map = upp::EnumMap<Int, int>{};
  map[Int::A] = 1;
  ASSERT_EQ(map.size(), 1);
  ASSERT_EQ(map.at(Int::A), 1);
  ASSERT_EQ(std::as_const(map).at(Int::A), 1);
}

