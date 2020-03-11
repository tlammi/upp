#include "upp/cli/parser.hpp"
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing;

TEST(ParserTest, BoolOpts) {
    bool flag{false};
    upp::cli::Parser p{"test",
                       [&](const upp::cli::Parser::ParsingData& parsed) -> int {
                           auto tmp = parsed.bool_options['a'];
                           EXPECT_TRUE(parsed.bool_options['a']);
                           EXPECT_TRUE(parsed.bool_options["AA"]);
                           EXPECT_FALSE(parsed.bool_options['b']);
                           EXPECT_FALSE(parsed.bool_options["BB"]);
                           flag = true;
                           return 0;
                       }};
    p.add_bool_options('a', "AA", "AA help", 'b', "BB", "BB help");
    p.parse({"demo", "-a"});
}
