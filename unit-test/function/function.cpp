#include "upp/function.hpp"

#include <gtest/gtest.h>

TEST(FunctionTest, Ctor) {
		upp::Func<void()> f0{[]() {}};
		upp::Func<int(int, double), 32> f1{
			[](int i, double d) -> int { return i * d; }};
}

TEST(FunctionTest, Invoke) {
		int i = 0;
		upp::Func<void(), 32> f0{[&]() { ++i; }};
		f0();
		ASSERT_EQ(i, 1);
}