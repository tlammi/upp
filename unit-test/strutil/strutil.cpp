#include "upp/strutil.hpp"

#include <gtest/gtest.h>

namespace strutil = upp::strutil;
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