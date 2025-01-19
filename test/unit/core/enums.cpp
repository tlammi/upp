
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

TEST(Map, Iterate) {
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
  iter->second = "A";
  ASSERT_EQ(map[Enum::A], "A");
  ++iter;
  ASSERT_EQ(iter->first, Enum::B);
  ASSERT_EQ(iter->second, "b");
  iter->second = "B";
  ASSERT_EQ(map[Enum::B], "B");
  ++iter;
  ASSERT_EQ(iter->first, Enum::D);
  ASSERT_EQ(iter->second, "d");
  iter->second = "D";
  ASSERT_EQ(map[Enum::D], "D");
  ++iter;
  ASSERT_EQ(iter, map.end());
}

TEST(Map, EraseOne) {
  auto map = upp::EnumMap<Int, int>();
  map[Int::A] = 1;
  auto count = map.erase(Int::A);
  ASSERT_EQ(count, 1);
  ASSERT_TRUE(map.empty());
}

TEST(Map, EraseMiss) {
  auto map = upp::EnumMap<Int, int>();
  map[Int::A] = 1;
  auto count = map.erase(Int::B);
  ASSERT_EQ(count, 0);
  ASSERT_EQ(map.size(), 1);
}
