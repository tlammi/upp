#include <gtest/gtest.h>

#include "upp/parser.hpp"

namespace p = upp::parser;

TEST(Parser, RegexNumber){
	std::string str{"900"};
	p::Factory factory =  p::factory(str.begin());

	bool match=false;
	auto integer = factory.regex(R"(0|[1-9][0-9]*)", [&](auto begin, auto end){
			std::string_view str{&*begin, static_cast<size_t>(end-begin)};
			if(str == "900")
				match = true;
	});

	auto dummy = factory.regex(R"(dummy)");

	auto res = integer.match(str.begin(), str.end());
	ASSERT_TRUE(res);
	ASSERT_TRUE(match);


	res = dummy.match(str.begin(), str.end());
	ASSERT_FALSE(res);
}


TEST(Parser, Joined){
	std::string str{R"("hello""world")"};
	auto factory = p::factory(str.begin());
	
	auto quoted = factory.regex(R"(".*?[^\\]")");
	auto two_strs = (quoted , quoted);

	auto match = two_strs.match(str.begin(), str.end());
	ASSERT_TRUE(match);
	ASSERT_EQ(match.iter, str.end());
}
