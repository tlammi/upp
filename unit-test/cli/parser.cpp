#include "upp/cli/parser.hpp"
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing;

TEST(ParserTest, BoolOpts) {
    upp::cli::Parser p{"test",
                       [](const upp::cli::Parser::ParsingData& parsed) -> int {
                           EXPECT_TRUE(parsed.bool_options['a']);
                           EXPECT_TRUE(parsed.bool_options["AA"]);
                           EXPECT_FALSE(parsed.bool_options['b']);
                           EXPECT_FALSE(parsed.bool_options["BB"]);
                           EXPECT_TRUE(parsed.bool_options["CC"]);
                           return 0;
                       }};
    p.add_bool_options('a', "AA", "AA help", 'b', "BB", "BB help", "CC",
                       "CC help");
    p.add_bool_option("DD", "DD help");
    p.parse({"demo", "-a", "--CC"});
}

TEST(ParserTest, Opts) {
    upp::cli::Parser p{"test",
                       [](const upp::cli::Parser::ParsingData& parsed) -> int {
                           EXPECT_EQ(parsed.options['a'].as<int>(), 100);
                           EXPECT_EQ(parsed.options["AA"].as<long>(), 100);
                           EXPECT_EQ(parsed.options["BB"].as<float>(), 2.0);
                           return 0;
                       }};

    p.add_options('a', "AA", "AA help", 'b', "BB", "BB help", "CC", "CC help");
    p.add_options("DD", "DD help");
    p.parse({"demo", "-a", "100", "--BB", "2.0"});
}

TEST(ParserTest, VectOpts) {
    upp::cli::Parser p{
        "test", [](const upp::cli::Parser::ParsingData& parsed) -> int {
            EXPECT_THAT(parsed.vector_options['1'].as<int>(),
                        ElementsAre(0, 1, 2));
            EXPECT_THAT(parsed.vector_options["option1"].as<short>(),
                        ElementsAre(0, 1, 2));
            EXPECT_THAT(parsed.vector_options["option2"].as<int>(),
                        ElementsAre(100, 101));
            return 0;
        }};

    p.add_vector_options('1', "option1", "help1", "option2", "help2");
    p.add_vector_option('3', "option3", "help3");

    p.parse({"test", "-1", "0", "-1", "1", "-1", "2", "--option2", "100",
             "--option2", "101"});
}