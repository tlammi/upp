#include <gtest/gtest.h>
#include "upp/property_tree.hpp"

namespace pt = upp::property_tree;

TEST(Value, CastInt){
	pt::Value val{100};
	ASSERT_EQ(val.cast<int>(), 100);
	ASSERT_DOUBLE_EQ(val.cast<double>(), 100.0);
	ASSERT_EQ(val.cast<std::string>(), "100");
}

TEST(Value, CastDouble){
	pt::Value val{100.5};
	ASSERT_EQ(val.cast<int>(), 100);
	ASSERT_DOUBLE_EQ(val.cast<double>(), 100.5);
	auto tmp = val.cast<std::string>();
	while(tmp.size() && tmp.back() == '0')
		tmp.pop_back();
	ASSERT_EQ(tmp, "100.5");
}

TEST(Value, CastString){
	pt::Value val{"100"};
	ASSERT_EQ(val.cast<int>(), 100);
	ASSERT_DOUBLE_EQ(val.cast<double>(), 100.0);
	ASSERT_EQ(val.cast<std::string>(), "100");
}
