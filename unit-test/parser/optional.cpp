
#include <gtest/gtest.h>

#include "fixture.hpp"

TEST_F(ParserFixture, OptionalCtor){
	auto c = factory.lit('c');
	auto opt0 = -c;
	auto opt1 = factory.ast(std::move(opt0), [](auto, auto){});
	auto opt2 = factory.ast(opt1);
}

TEST_F(ParserFixture, OptionalMatch){
	auto a = factory.lit('a');

	auto opt0 = -a;

	bool called = false;
	auto opt1 = factory.ast(opt0, [&](auto, auto){ called = true; });

	std::string str0{"hello"};
	std::string str1{"aaaaa"};

	auto res = upp::parser::parse(str0.cbegin(), str0.cend(), opt0);
	ASSERT_TRUE(res);
	ASSERT_EQ(res.iter, str0.cbegin());

	res = upp::parser::parse(str1.cbegin(), str1.cend(), opt1);

	ASSERT_TRUE(res);
	ASSERT_TRUE(called);
	ASSERT_EQ(res.iter, str1.cbegin()+1);
}
