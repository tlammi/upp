
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

TEST(Map, StaticAdd) {
  auto map = upp::EnumMapStatic<Int, int>{};
  map[Int::A] = 1;
  ASSERT_EQ(map.size(), 1);
  ASSERT_EQ(map.at(Int::A), 1);
  ASSERT_EQ(std::as_const(map).at(Int::A), 1);
}

TEST(Map, ConstIterate) {
  enum class Enum {
    A,
    B,
    C,
    D,
  };

  auto map = upp::EnumMap<Enum, std::string>();
  map[Enum::A] = "a";
  map[Enum::B] = "b";
  map[Enum::D] = "d";

  auto iter = map.begin();
  ASSERT_EQ(iter->first, Enum::A);
  ASSERT_EQ(iter->second, "a");
  ++iter;
  ASSERT_EQ(iter->first, Enum::B);
  ASSERT_EQ(iter->second, "b");
  ++iter;
  ASSERT_EQ(iter->first, Enum::D);
  ASSERT_EQ(iter->second, "d");
  ++iter;
  ASSERT_EQ(iter, map.end());

  /*
  for (const auto& [key, val] : map) {
  }
  */
}
