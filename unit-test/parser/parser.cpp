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


TEST(Parser, JoinedRef){
	std::string str{R"("hello""world")"};
	auto factory = p::factory(str.begin());

	std::string match1;
	std::string match2;
	
	auto quoted = factory.regex(R"(".*?[^\\]")", [&](auto begin, auto end){
		if(match1 == "") match1 = std::string{begin, end};
		else match2 = std::string{begin, end};
	});

	auto two_strs = (quoted , quoted);

	auto match = two_strs.match(str.begin(), str.end());
	ASSERT_TRUE(match);
	ASSERT_EQ(match.iter, str.end());
	ASSERT_EQ(match1, "\"hello\"");
	ASSERT_EQ(match2, "\"world\"");
}

TEST(Parser, JoinedMove){
	std::string str{R"("hello""world")"};
	auto factory = p::factory(str.begin());
	
	std::string_view restr = R"(".*?[^\\]")";
	auto two_strs = (factory.regex(restr) , factory.regex(restr));

	auto match = two_strs.match(str.begin(), str.end());
	ASSERT_TRUE(match);
	ASSERT_EQ(match.iter, str.end());

}
