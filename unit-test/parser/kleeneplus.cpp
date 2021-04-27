
#include <gtest/gtest.h>

#include "fixture.hpp"

TEST_F(ParserFixture, KleenePlusCtor){
	auto a = factory.lit('a');
	auto xa0 = +a;
	auto xa1 = factory.ast(std::move(xa0), [](auto, auto){});
	auto xa2 = factory.ast(xa1);
}


TEST_F(ParserFixture, KleenePlusMatch){
	auto a = factory.lit('a');
	auto xa = +a;
	bool called=false;
	auto xa_cb = factory.ast(xa, [&](auto, auto){ called = true; });

	std::string str{"aaab"};

	auto res = upp::parser::parse(str.cbegin(), str.cend(), xa);
	ASSERT_TRUE(res);
	ASSERT_EQ(res.iter, str.cbegin()+3);


	res = upp::parser::parse(str.cbegin(), str.cend(), xa_cb);
	ASSERT_TRUE(res);
	ASSERT_TRUE(called);
}


TEST_F(ParserFixture, KleenePlusMiss){
	auto a = factory.lit('a');
	auto xa = +a;
	bool called=false;
	auto xa_cb = factory.ast(xa, [&](auto, auto){ called = true; });

	std::string str{"baaab"};

	auto res = upp::parser::parse(str.cbegin(), str.cend(), xa);
	ASSERT_FALSE(res);
	ASSERT_EQ(res.iter, str.cbegin());


	res = upp::parser::parse(str.cbegin(), str.cend(), xa_cb);
	ASSERT_FALSE(res);
	ASSERT_FALSE(called);
}
