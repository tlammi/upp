
#include <gtest/gtest.h>

#include "fixture.hpp"



TEST_F(ParserFixture, KleeneStarCtor){
	auto a = factory.lit('a');
	auto xa0 = *a;
	auto xa1 = factory.ast(std::move(xa0), [](auto, auto){});
	auto xa2 = factory.ast(xa1);
}


TEST_F(ParserFixture, KleeneStarMatch){
	auto a = factory.lit('a');
	auto xa = *a;

	bool called=false;
	auto xa_cb = factory.ast(xa, [&](auto, auto){ called = true; });


	std::string str0{"aaaaa"};
	std::string str1{"baaaa"};
	
	auto res = upp::parser::parse(str0.cbegin(), str0.cend(), xa);
	ASSERT_TRUE(res);
	ASSERT_EQ(res.iter, str0.cend());

	res = upp::parser::parse(str1.cbegin(), str1.cend(), xa_cb);

	ASSERT_TRUE(res);
	ASSERT_TRUE(called);
	ASSERT_EQ(res.iter, str1.cbegin());

}
