#include <gtest/gtest.h>

#include <upp/util.hpp>

TEST(Match, Single) {
  auto var = std::variant<int>(1);
  auto res = upp::match(var, [](int i) { return i; });
  ASSERT_EQ(res, 1);
}

TEST(Match, Second) {
  auto var = std::variant<int, float>(std::in_place_type<float>, 1.0);
  auto res = upp::match(
      var, [](int i) { return i; },
      [](float f) { return static_cast<int>(f) * 2; });
  ASSERT_EQ(res, 2);
}

TEST(Match, ReturnVoid) {
  auto var = std::variant<int>(1);
  int res = 0;
  upp::match(var, [&](int i) { res = i; });
  ASSERT_EQ(res, 1);
}
