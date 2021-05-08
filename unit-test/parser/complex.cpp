
#include <gtest/gtest.h>
#include <gmock/gmock.h>

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

const std::string LIST = "1,2,3,4,5,6, 8, 11111";

TEST_F(ParserFixture, RecursiveList){
	
	std::vector<std::string> items;

	auto list = factory.dyn_ast();
	auto item = factory.regex("[^,]+", [&](auto begin, auto end){
			items.emplace_back(begin, end);
	});

	list = (item, -(factory.lit(','), list));

	auto res = upp::parser::parse(LIST.cbegin(), LIST.cend(), list, upp::parser::whitespaces);
	ASSERT_TRUE(res);
	ASSERT_EQ(items.size(), 8);
}

const std::string LIST_2 = "[1,2 3]";

TEST_F(ParserFixture, Error){
	
	auto item = factory.regex(R"([^\s,]+)");

	auto list = (factory.lit('['), item, *(factory.lit(','), item), factory.lit(']'));

	auto res = upp::parser::parse(LIST_2.cbegin(), LIST_2.cend(), list, upp::parser::whitespaces);
	ASSERT_FALSE(res);
	auto err_str = upp::parser::error_msg(LIST_2.cbegin(), LIST_2.cend(), res);
	ASSERT_NE(err_str.find("1:6"), std::string::npos);

}



const std::string INVALID_LIST = "[1, 2, 3";

// Check that the iterator is left where the best match left it
TEST_F(ParserFixture, DoubleError){
	auto item = factory.regex(R"([^\s,]+)");
	auto list = (factory.lit('['), item, *(factory.lit(','), item), factory.lit(']'));

	// Mock rule to test that the iterator does not get cleared
	auto something_else = (factory.lit('['), factory.lit('['));

	// Matches list first and then the mock rule. Iterator should be left
	// where the left hand operand leaves it as it is further
	auto ast = (list | something_else);

	auto res = upp::parser::parse(INVALID_LIST.cbegin(), INVALID_LIST.cend(), ast, upp::parser::whitespaces);
	ASSERT_FALSE(res);
	auto distance = res.iter - INVALID_LIST.cbegin();
	ASSERT_EQ(distance, 8);
	ASSERT_EQ(res.expecteds.size(), 2);

	auto expected_names = [&](){
		std::vector<std::string_view> names;
		for(const auto& exp: res.expecteds)
			names.push_back(exp->name());
		return names;
	};

	auto names = expected_names();

	ASSERT_THAT(names, testing::UnorderedElementsAre("]", ","));

}
