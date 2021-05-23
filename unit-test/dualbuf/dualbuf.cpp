#include "upp/dualbuf.hpp"

#include <gtest/gtest.h>
#include <vector>


TEST(DualBuf, Ctor){
	upp::DualBuf<int> d0;
	upp::DualBuf<std::vector<int>> d1;
}


TEST(DualBuf, Swap){
	upp::DualBuf<int> d0;
	(*d0.front()) = 100;
	d0.swap();
	ASSERT_EQ(*d0.back(), 100);
	(*d0.front()) = 99;
	d0.swap();
	ASSERT_EQ(*d0.back(), 99);
	ASSERT_EQ(*d0.front(), 100);
}
