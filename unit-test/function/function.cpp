#include "upp/function.hpp"

#include <gtest/gtest.h>
#include <array>

int func(double d, float f) { return d + f; }
TEST(FunctionTest, Ctor) {
		upp::Function f0{[]() {}};
		static_assert(
			std::is_same_v<decltype(f0)::prototype_t, void()>
		);
		auto lambda0 = [](){};
		ASSERT_EQ(f0.used_size(), sizeof(lambda0));

		upp::Function<int(int, double), 32> f1{
			[](int i, double d) -> int { return i * d; }};
		static_assert(
			std::is_same_v<decltype(f1)::prototype_t, int(int, double)>
		);
		auto lambda1 = [](int i, double d) -> int {return i*d;};
		ASSERT_EQ(f1.used_size(), sizeof(lambda1));

		upp::Function f2{func};
		static_assert(
			std::is_same_v<decltype(f2)::prototype_t, int(double, float)>
		);
		ASSERT_EQ(f2.used_size(), sizeof(&func));
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
		auto lambda = [](double d) -> int{return d*100;};
		ASSERT_EQ(f.used_size(), sizeof(lambda));
}

TEST(FunctionTest, TooBig){
	auto lambda = [a = std::array<char, 36>()](double d) -> int{
		return d;
	};

	upp::Function<int(double), 64> f{
		std::move(lambda)
	};

	auto tmp = [](upp::Function<int(double), 64>&& f){
		upp::Function<int(double), 32> f2{std::move(f)};
	};

	ASSERT_ANY_THROW(tmp(std::move(f)));
}

TEST(FunctionTest, Moving){
	auto lambda = [a = std::array<char, 10>()](int i) -> int{
		return i;
	};
	constexpr size_t lambda_size = sizeof(lambda);
	upp::Function f0{std::move(lambda)};
	ASSERT_EQ(f0.used_size(), lambda_size);
	
	
	upp::Function f1{std::move(f0)};
	ASSERT_EQ(f1.used_size(), lambda_size);

	upp::Function<int(int), 64> f2{std::move(f1)};
	ASSERT_EQ(f2.used_size(), lambda_size);
	
	f0 = std::move(f2);

	ASSERT_EQ(f0.used_size(), lambda_size);

}

