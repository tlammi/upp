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

TEST(Parser, Optional){
	std::string str0{"1234567890"};
	std::string str1{""};
	auto factory = p::factory(str0.begin());

	int count=0;
	auto integer = factory.regex(R"([0-9])", [&](auto begin, auto end){
		(void)begin;
		(void)end;
		++count;
	});

	auto optional = -(integer);

	auto res = optional.match(str0.begin(), str0.end());

	ASSERT_TRUE(res);
	ASSERT_EQ(count, 1);
	ASSERT_EQ(res.iter, str0.begin()+1);

	res = optional.match(str1.begin(), str1.end());
	ASSERT_TRUE(res);
}

TEST(Parser, Literal){
	std::string str{"hello, world"};
	std::string str2{"this is a str"};

	auto factory = p::factory(str.begin());
	auto lit0 = factory.lit('h');
	auto res0 = lit0.match(str.begin(), str.end());
	ASSERT_TRUE(res0);
	ASSERT_EQ(res0.iter, str.begin()+1);

	res0 = lit0.match(str2.begin(), str2.end());
	ASSERT_FALSE(res0);
	ASSERT_EQ(res0.iter, str2.begin());


	auto lit1 = factory.lit("hell");
	auto res1 = lit1.match(str.begin(), str.end());
	ASSERT_TRUE(res1);
	ASSERT_EQ(res1.iter, str.begin()+4);

	res1 = lit1.match(str2.begin(), str2.end());
	ASSERT_FALSE(res1);
	ASSERT_EQ(res1.iter, str2.begin());
}


TEST(Parser, Recurse){
	std::string str{R"(
{
	"key0": 1,
	"key1": {
		"subkey1": "hello",
		"subkey2": 10
	},
	"key2": "str"
}
	)"};

	auto factory = p::factory(str.begin());
	auto obj = factory.dyn_ast();
	auto integer = factory.regex(R"(0|[1-9][0-9]*)");
	auto quoted = factory.regex(R"(".*?[^\\]")");
	auto key_value = (quoted, factory.lit(':'), (integer | quoted | obj));
	obj = (factory.lit('{'),-(key_value), *(factory.lit(','), key_value), factory.lit('}'));
	
	auto res = obj.match(str.begin(), str.end(), p::whitespaces);
	ASSERT_TRUE(res);
}
