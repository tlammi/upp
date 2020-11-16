#include "upp/impl/traits/callable_prototype.hpp"

#include <gtest/gtest.h>

int foo(int i){
	return i*i;
}

constexpr double bar(int i, float f){
	return f*i;
}



static_assert(std::is_same_v<upp::traits::callable_prototype<decltype(foo)>::type, int(int)>);
static_assert(
	std::is_same_v<upp::traits::callable_prototype<decltype(bar)>::type, double(int, float)>);


TEST(CallablePrototype, Lambda){
	auto lambda0 = [i = 100](char c)-> double{
		return i*c;
	};
	static_assert(
		std::is_same_v<
			upp::traits::callable_prototype<decltype(lambda0)>::type,
			double(char)>
		);
	auto lambda1 = [](){};
	static_assert(
		std::is_same_v<
			upp::traits::callable_prototype<decltype(lambda1)>::type,
			void()>
		);
}
