#include "upp/impl/util/defer.hpp"

#include <gtest/gtest.h>
using namespace upp::util;

TEST(UtilDeferTest, Call) {
		int i = 0;
		{
				Defer d{[&]() noexcept { i = 3; }};
		}
		ASSERT_EQ(i, 3);
}