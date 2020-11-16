#include "upp/function.hpp"

#include <gtest/gtest.h>

int func(double d, float f) { return d + f; }
TEST(FunctionTest, Ctor) {
		upp::Function f0{[]() {}};
		static_assert(
			std::is_same_v<decltype(f0)::prototype_t, void()>
		);
		upp::Function<int(int, double), 32> f1{
			[](int i, double d) -> int { return i * d; }};
		upp::Function f2{func};
		static_assert(
			std::is_same_v<decltype(f2)::prototype_t, int(double, float)>
		);
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
