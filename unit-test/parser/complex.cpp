
#include <gtest/gtest.h>

#include "fixture.hpp"

const std::string C_FUNC = R"(
int foobar(void){
	return 42;
})";

TEST_F(ParserFixture, ValidCFunc){
	std::string ret_type_str;
	std::string func_name_str;
	std::string params_str;
	std::string body_str;

	auto ret_type = factory.regex(R"([a-zA-Z][a-zA-Z0-9_]*)", [&](auto begin, auto end){
		ret_type_str = std::string(begin, end);
	});


	auto func_name = factory.regex(R"([a-zA-Z][a-zA-Z0-9]*)", [&](auto begin, auto end){
		func_name_str = std::string(begin, end);
	});

	auto params = factory.regex(R"([^)]*)", [&](auto begin, auto end){
		params_str = std::string(begin, end);
	});

	auto body = factory.regex(R"([^}]*)", [&](auto begin, auto end){
		body_str = std::string(begin, end);
	});

	auto func = (ret_type, func_name, factory.lit('('), params, factory.lit(')'), factory.lit('{'), body, factory.lit('}'));

	auto res = upp::parser::parse(C_FUNC.cbegin(), C_FUNC.cend(), func, upp::parser::whitespaces);
	ASSERT_EQ(ret_type_str, "int");
	ASSERT_EQ(func_name_str, "foobar");
	ASSERT_EQ(params_str, "void");
	ASSERT_NE(body_str.find("return 42;"), std::string::npos);
	ASSERT_TRUE(res);
	ASSERT_EQ(C_FUNC.cend()-res.iter, 0);
}
