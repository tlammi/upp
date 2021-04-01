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


TEST(Node, IndexOperator){
	pt::Node n = pt::Dict();
	n["0"] = "A";
	n["1"] = 100;
	n["2"] = 100.3;

	pt::Node n2 = pt::List();
	n2.list().emplace_back();
	n2.list().emplace_back();
	n2.list().emplace_back();

	n2[0] = "A";
	n2[1] = 100;
	n2[2] = 100.3;

	ASSERT_EQ(*n["0"], "A");
	ASSERT_EQ(*n["1"], 100);
	ASSERT_DOUBLE_EQ(n["2"]->get<double>(), 100.3);

	ASSERT_EQ(n["0"], n2[0]);
	ASSERT_EQ(n["1"], n2[1]);

	ASSERT_DOUBLE_EQ(n2[2]->get<double>(), 100.3);
}

