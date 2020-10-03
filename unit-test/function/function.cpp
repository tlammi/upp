#include "upp/function.hpp"

#include <gtest/gtest.h>

int func(double d, float f) { return d + f; }
TEST(FunctionTest, Ctor) {
		upp::Function f0{[]() {}};
		upp::Function<int(int, double), 32> f1{
			[](int i, double d) -> int { return i * d; }};
		upp::Function f2{func};

		auto f3 = upp::make_function([]() {});
}

TEST(FunctionTest, Invoke) {
		int i = 0;
		upp::Function<void(), 32> f0{[&]() { ++i; }};
		f0();
		ASSERT_EQ(i, 1);
}

TEST(FunctionTest, SmallerToBiggerCtor) {
		upp::Function<void(), 32> f0{[]() {}};
		ASSERT_EQ(sizeof(f0) - sizeof(size_t), 32);
		upp::Function<void(), 64> f1{std::move(f0)};
		ASSERT_EQ(sizeof(f1) - sizeof(size_t), 64);
}
TEST(FunctionTest, SmallerToBiggerAssign) {
		upp::Function<void(), 32> f0{[]() {}};
		ASSERT_EQ(sizeof(f0) - sizeof(size_t), 32);
		upp::Function<void(), 64> f1{};
		f1 = std::move(f0);
		ASSERT_EQ(sizeof(f1) - sizeof(size_t), 64);
}

TEST(FunctionTest, BiggerToSmaller) {
		upp::Function<void(), 64> f0{[]() {}};
		ASSERT_EQ(sizeof(f0) - sizeof(size_t), 64);
		upp::Function<void()> f1{std::move(f0)};
		ASSERT_EQ(sizeof(f1) - sizeof(size_t), 32);
}