#include "upp/strutil.hpp"

#include <gtest/gtest.h>

namespace strutil = upp::strutil;

TEST(StrUtil, Strip) {
		static_assert(strutil::strip("   hello, world   \n") == "hello, world");
		static_assert(strutil::strip("") == "");
		static_assert(strutil::strip("h") == "h");
		static_assert(strutil::strip("\n\n\n\nhh") == "hh");
		static_assert(strutil::strip("hh hh") == "hh hh");
}
TEST(StrUtil, IsSpace) {
		static_assert(strutil::isspace(' '));
		static_assert(strutil::isspace('\n'));
		static_assert(!strutil::isspace('9'));
}

TEST(StrUtil, IsDigit) {
		for (size_t i = 0; i < 10; ++i) {
				ASSERT_TRUE(strutil::isdigit(i + '0'));
		}
		static_assert(!strutil::isdigit('a'));
}

TEST(StrUtil, ToDigit) {
		static_assert(9 == strutil::todigit('9'));
		for (char i = 0; i < 10; ++i) {
				ASSERT_EQ(i, strutil::todigit(i + '0'));
		}
		ASSERT_ANY_THROW(strutil::todigit('-'));
}

TEST(StrUtil, ToNum) {
		static_assert(strutil::tonum<int>("10") == 10);
		static_assert(strutil::tonum<double>("10.1") == 10.1);
		ASSERT_ANY_THROW(strutil::tonum<int>("1.1.1"));
		ASSERT_ANY_THROW(strutil::tonum<float>("asdf"));
		strutil::tonum<float>(
			"10.1111111111111111111111123123412453454132523452345");
}