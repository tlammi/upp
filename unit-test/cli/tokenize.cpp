#include <gtest/gtest.h>

#include "upp/cli/token.hpp"

using namespace upp::cli;

TEST(CliTest, TokenizeSflag) {
		const char* args[] = {"-asdfg"};
		auto tokens = tokenize(args[0]);

		ASSERT_EQ(tokens.size(), 5);

		for (const auto& t : tokens) {
				ASSERT_EQ(t.type, TokenType::ShortFlag);
		}

		ASSERT_EQ(tokens[0].ptr[0], 'a');
		ASSERT_EQ(tokens[0].ptr[1], 's');
		ASSERT_EQ(tokens[0].ptr[2], 'd');
		ASSERT_EQ(tokens[0].ptr[3], 'f');
		ASSERT_EQ(tokens[0].ptr[4], 'g');
}

TEST(CliTest, TokenizeLflag) {
		const char* args[] = {"--flag"};
		auto tokens = tokenize(args[0]);
		ASSERT_EQ(tokens.size(), 1);
		ASSERT_EQ(std::string("flag"), tokens[0].ptr);
		ASSERT_EQ(tokens[0].type, TokenType::LongFlag);
}

TEST(CliTest, TokenizeOther) {
		const char* args[] = {"other-option-----with-----dashes----"};
		auto tokens = tokenize(args[0]);
		ASSERT_EQ(tokens.size(), 1);
		ASSERT_EQ(tokens[0].type, TokenType::Other);
		ASSERT_EQ(std::string(args[0]), tokens[0].ptr);
}