
#include <gtest/gtest.h>

#include "fixture.hpp"


TEST_F(ParserFixture, OrCtor){
	auto str = factory.lit("str");
	auto b = factory.lit('b');

	auto or0 = str | b;
	auto or1 = factory.ast(or0, [](auto, auto){});
}

TEST_F(ParserFixture, OrMatch){

	auto a = factory.lit('a');
	auto b = factory.lit('b');

	auto o = a | *b;

	bool called = false;
	auto o_cb = factory.ast(o, [&](auto, auto){ called = true; });

	std::string str0{"aaaa"};
	std::string str1{"bbbbbb"};

	auto res = upp::parser::parse(str0.cbegin(), str0.cend(), o);
	ASSERT_TRUE(res);
	ASSERT_EQ(res.iter, str0.cbegin()+1);
	res = upp::parser::parse(str1.cbegin(), str1.cend(), o);
	ASSERT_TRUE(res);
	ASSERT_EQ(res.iter, str1.cend());

}


TEST_F(ParserFixture, OrMiss){
	auto a = factory.lit('a');
	auto b = factory.lit('b');

	auto o = a | *b;

	bool called = false;
	auto o_cb = factory.ast(o, [&](auto, auto){ called = true; });

	std::string str{"cccccc"};

	auto res = upp::parser::parse(str.cbegin(), str.cend(), o);
	ASSERT_TRUE(res);
	ASSERT_EQ(res.iter, str.cbegin());
}

