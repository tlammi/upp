#include <gtest/gtest.h>

#include <upp/str.hpp>

TEST(Util, StrCmpEq) {
  auto res = upp::strcmp("foo", "foo");
  ASSERT_EQ(res, 0);
}

TEST(Util, StrCmpLess) {
  auto res = upp::strcmp("foa", "foo");
  ASSERT_LT(res, 0);
}

TEST(Util, StrCmpGreater) {
  auto res = upp::strcmp("foo", "foa");
  ASSERT_GT(res, 0);
}

TEST(Util, StrCmpShorter) {
  auto res = upp::strcmp("fo", "foo");
  ASSERT_LT(res, 0);
}

TEST(Util, StrCmpLonger) {
  auto res = upp::strcmp("foo", "fo");
  ASSERT_GT(res, 0);
}

TEST(CStr, Default) {
  auto s = upp::CString();
  ASSERT_EQ(s.size(), 0);
  ASSERT_EQ(s.data(), nullptr);
}

TEST(CStr, StrLiteral) {
  auto s = upp::CString("foo");
  ASSERT_EQ(s.length(), 3);
  ASSERT_EQ(s, "foo");
  auto str = std::string("foo");
  ASSERT_EQ(s, str.c_str())
      << "CString implicitly converted to const char* in comparison";
  str.pop_back();
  ASSERT_NE(s, str.c_str())
      << "CString implicitly converted to const char* in comparison";
}
