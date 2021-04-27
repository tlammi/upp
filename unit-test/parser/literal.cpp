
#include <gtest/gtest.h>

#include "fixture.hpp"

TEST_F(ParserFixture, CharLiteralCtor){
	upp::parser::LiteralChar<std::string::const_iterator> c{'c'};
	upp::parser::LiteralChar<std::string::const_iterator,
		void(*)(std::string::const_iterator, std::string::const_iterator)>
		a{'a', [](auto, auto){}};
}


TEST_F(ParserFixture, CharLiteralMatch){
	bool called = false;

	auto a = factory.lit('a');
	auto a_cb = factory.lit('a', [&](auto, auto){ called = true; });

	std::string str{"asdf"};

	auto res = upp::parser::parse(str.cbegin(), str.cend(), a);
	ASSERT_TRUE(res);
	ASSERT_EQ(res.iter, str.cbegin()+1);
	ASSERT_TRUE(upp::parser::parse(str.cbegin(), str.cend(), a_cb));
	ASSERT_TRUE(called);
}


TEST_F(ParserFixture, CharLiteralMiss){
	bool called = false;

	auto a = factory.lit('a');
	auto a_cb = factory.lit('a', [&](auto, auto){ called = true; });

	std::string str{"basdf"};

	auto res = upp::parser::parse(str.cbegin(), str.cend(), a);
	ASSERT_FALSE(res);
	ASSERT_EQ(res.iter, str.cbegin());
	ASSERT_FALSE(upp::parser::parse(str.cbegin(), str.cend(), a_cb));
	ASSERT_FALSE(called);
}

TEST_F(ParserFixture, StrLiteralMatch){
	bool called = false;

	auto a = factory.lit("asd");
	auto a_cb = factory.lit("asd", [&](auto, auto){ called = true; });

	std::string str{"asdf"};

	auto res = upp::parser::parse(str.cbegin(), str.cend(), a);
	ASSERT_TRUE(res);
	ASSERT_EQ(res.iter, str.cbegin()+3);
	ASSERT_TRUE(upp::parser::parse(str.cbegin(), str.cend(), a_cb));
	ASSERT_TRUE(called);
}


TEST_F(ParserFixture, StrLiteralMiss){
	bool called = false;

	auto a = factory.lit("asdg");
	auto a_cb = factory.lit("asdg", [&](auto, auto){ called = true; });

	std::string str{"asdf"};

	auto res = upp::parser::parse(str.cbegin(), str.cend(), a);
	ASSERT_FALSE(res);
	ASSERT_EQ(res.iter, str.cbegin());
	ASSERT_FALSE(upp::parser::parse(str.cbegin(), str.cend(), a_cb));
	ASSERT_FALSE(called);
}
