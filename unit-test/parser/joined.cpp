
#include <gtest/gtest.h>

#include "fixture.hpp"


TEST_F(ParserFixture, JoinedCtor){
	auto a = factory.lit('a');
	auto c = factory.lit('c');
	auto j0 = (std::move(a), c);
}

TEST_F(ParserFixture, JoinedMatch){
	auto a = factory.lit('a');
	auto b = factory.lit('b');
	auto j = (a, b);
	bool called=false;
	auto j_cb = factory.ast(j, [&](auto, auto){ called = true; });

	std::string str{"abcdefg"};

	auto res = upp::parser::parse(str.cbegin(), str.cend(), j);
	ASSERT_TRUE(res);

	ASSERT_EQ(res.iter, str.cbegin()+2);

	ASSERT_TRUE(upp::parser::parse(str.cbegin(), str.cend(), j_cb));
	ASSERT_TRUE(called);
}


TEST_F(ParserFixture, JoinedMiss){
	auto a = factory.lit('a');
	auto b = factory.lit('b');
	auto j = (a, b);
	bool called=false;
	auto j_cb = factory.ast(j, [&](auto, auto){ called = true; });

	std::string str{"accdefg"};

	auto res = upp::parser::parse(str.cbegin(), str.cend(), j);
	ASSERT_FALSE(res);
	ASSERT_EQ(res.iter, str.cbegin()+1);
}
