
#include <gtest/gtest.h>

#include "fixture.hpp"

TEST_F(ParserFixture, RegexCtor){
	upp::parser::Regex<std::string::const_iterator> hex{R"(0x[0-f]+)"};
	upp::parser::Regex<std::string::const_iterator, void(*)(std::string::const_iterator, std::string::const_iterator)>
		base10{R"(0|[1-9][0-9]*)", [](auto, auto){}};
}

TEST_F(ParserFixture, RegexMatch){

	bool called=false;

	auto re = factory.regex(R"(a*)");
	auto re_cb = factory.ast(re, [&](auto, auto){ called = true; });

	std::string str{"aaaaaaaaa"};
	ASSERT_TRUE(upp::parser::parse(str.cbegin(), str.cend(), re));
	ASSERT_TRUE(upp::parser::parse(str.cbegin(), str.cend(), re_cb));

	ASSERT_TRUE(called);
}

TEST_F(ParserFixture, RegexMiss){
	bool called=false;

	auto re = factory.regex(R"(a+)");
	auto re_cb = factory.ast(re, [&](auto, auto){ called = true; });

	std::string str{"baaaaaaaaa"};
	ASSERT_FALSE(upp::parser::parse(str.cbegin(), str.cend(), re));
	ASSERT_FALSE(upp::parser::parse(str.cbegin(), str.cend(), re_cb));

	ASSERT_FALSE(called);
}

