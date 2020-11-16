#include "upp/function.hpp"

#include <gtest/gtest.h>

int func(double d, float f) { return d + f; }
TEST(FunctionTest, Ctor) {
		upp::Function<void()> f0{[]() {}};
		upp::Function<int(int, double), 32> f1{
			[](int i, double d) -> int { return i * d; }};
		upp::Function<int(double, float)> f2{func};
}

TEST(FunctionTest, Invoke) {
		int i = 0;
		upp::Function<void(), 32> f0{[&]() { ++i; }};
		f0();
		ASSERT_EQ(i, 1);
}

TEST(FunctionTest, SmallerToBiggerCtor) {
		upp::Function<void(), 32> f0{[]() {}};
		upp::Function<void(), 64> f1{std::move(f0)};
}
TEST(FunctionTest, SmallerToBiggerAssign) {
		upp::Function<void(), 32> f0{[]() {}};
		upp::Function<void(), 64> f1{};
		f1 = std::move(f0);
}

TEST(FunctionTest, BiggerToSmaller) {
		upp::Function<void(), 64> f0{[]() {}};
		upp::Function<void()> f1{std::move(f0)};
}

TEST(FunctionTest, Assign) {
		upp::Function<int(double), 32> f{
			[](double d) -> int { return d + 100; }};
		f = [](double d) -> int { return d * 100; };
}
