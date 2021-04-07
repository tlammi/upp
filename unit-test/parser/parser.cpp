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

TEST(Parser, Or){
	
	std::string int_str{"420"};
	std::string str_str{R"("hello")"};
	std::string nomatch{"asdasdf"};
	auto factory = p::factory(int_str.begin());
	
	auto int_or_str = factory.regex(R"(0|[1-9][0-9]*)") | factory.regex(R"(".*?[^\\]")");

	auto match = int_or_str.match(int_str.begin(), int_str.end());
	ASSERT_TRUE(match);
	ASSERT_EQ(match.iter, int_str.end());
	
	match = int_or_str.match(str_str.begin(), str_str.end());

	ASSERT_TRUE(match);
	ASSERT_EQ(match.iter, str_str.end());


	match = int_or_str.match(nomatch.begin(), nomatch.end());
	ASSERT_FALSE(match);
	ASSERT_EQ(match.iter, nomatch.begin());
}

TEST(Parser, KleeneStar){
	std::string str0{"1234567890"};
	std::string str1{""};
	auto factory = p::factory(str0.begin());

	int count=0;
	auto integer = factory.regex(R"([0-9])", [&](auto begin, auto end){
		(void)begin;
		(void)end;
		++count;
	});

	auto ints = *(integer);

	auto res = ints.match(str0.begin(), str0.end());

	ASSERT_TRUE(res);
	ASSERT_EQ(count, 10);

	res = ints.match(str1.begin(), str1.end());
	ASSERT_TRUE(res);
	ASSERT_EQ(count, 10);

}


TEST(Parser, KleenePlus){
	std::string str0{"1234567890"};
	std::string str1{""};
	auto factory = p::factory(str0.begin());

	int count=0;
	auto integer = factory.regex(R"([0-9])", [&](auto begin, auto end){
		(void)begin;
		(void)end;
		++count;
	});

	auto ints = +(integer);

	auto res = ints.match(str0.begin(), str0.end());

	ASSERT_TRUE(res);
	ASSERT_EQ(count, 10);

	res = ints.match(str1.begin(), str1.end());
	ASSERT_FALSE(res);
	ASSERT_EQ(count, 10);


}

