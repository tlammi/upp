#include "upp/cli/parser.hpp"
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>

using upp::cli::Parser;

using namespace ::testing;

TEST(ParserTest, BoolOpts) {
    Parser p{"test", [](const Parser::ParsingData& parsed) -> int {
                 EXPECT_TRUE(parsed.bool_options['a']);
                 EXPECT_TRUE(parsed.bool_options["AA"]);
                 EXPECT_FALSE(parsed.bool_options['b']);
                 EXPECT_FALSE(parsed.bool_options["BB"]);
                 EXPECT_TRUE(parsed.bool_options["CC"]);
                 return 0;
             }};
    p.add_bool_options('a', "AA", "AA help", 'b', "BB", "BB lp", "CC",
                       "CC help");
    p.add_bool_option("DD", "DD help");
    p.parse({"demo", "-a", "--CC"});
}

TEST(ParserTest, Opts) {
    Parser p{"test", [](const Parser::ParsingData& parsed) -> int {
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
    Parser p{"test", [](const Parser::ParsingData& parsed) -> int {
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

TEST(ParserTest, SubCommand) {
    int counter{0};
    Parser p{"test", [&](const Parser::ParsingData& parsed) -> int {
                 EXPECT_EQ(counter, 0);
                 ++counter;
                 return 0;
             }};

    auto& subcmd = p.add_subcommand(
        "subcmd", "help", [&](const Parser::ParsingData& parsed) -> int {
            EXPECT_EQ(counter, 1);
            ++counter;
            return 0;
        });
    p.add_subcommand("subcmd2", "help2", [](const Parser::ParsingData& parsed) {
        ADD_FAILURE();
        return 0;
    });
    subcmd.add_subcommand("subcmd2", "help22",
                          [&](const Parser::ParsingData& parsed) {
                              EXPECT_EQ(counter, 2);
                              ++counter;
                              return 0;
                          });

    p.parse({"test", "subcmd", "subcmd2"});
    EXPECT_EQ(counter, 3);
}

TEST(ParserTest, BoolOptsMultiple) {
    Parser p{"test", [](const Parser::ParsingData& parsed) { return 0; }};
    p.add_bool_option('f', "flag", "asdf");
    ASSERT_THROW(p.add_bool_option('f', "flag", "help"), std::logic_error);
    ASSERT_THROW(p.add_bool_option("flag", "help string"), std::logic_error);
}

TEST(ParserTest, OptsMultiple) {
    Parser p{"test", [](const Parser::ParsingData& parsed) { return 0; }};
    p.add_option('o', "option", "asdf");
    ASSERT_THROW(p.add_option('o', "other-option", "asdf"), std::logic_error);
    ASSERT_THROW(p.add_option("option", "asdf"), std::logic_error);
}

TEST(ParserTest, VectOptsMultiple) {
    Parser p{"test", [](const Parser::ParsingData& parsed) { return 0; }};
    p.add_vector_option('v', "vector-option", "asdf");
    ASSERT_THROW(p.add_vector_option('v', "vect", "asdfasdf"),
                 std::logic_error);
    ASSERT_THROW(p.add_vector_option("vector-option", "a"), std::logic_error);
}

TEST(ParserTest, OptionConflict) {
    Parser p{"", [](const auto& parsed) { return 0; }};
    p.add_bool_option('o', "option", "");
    ASSERT_THROW(p.add_option('o', "asdf", ""), std::logic_error);
    ASSERT_THROW(p.add_vector_option("option", "asdfasdf"), std::logic_error);
}

TEST(ParserTest, SameOptionOnMultipleLevels) {
    Parser p{"", [](const Parser::ParsingData& parsed) {
                 EXPECT_EQ(parsed.options["option"].as<int>(), 1);
                 return 0;
             }};
    auto& subcmd =
        p.add_subcommand("subcmd", "", [](const Parser::ParsingData& parsed) {
            EXPECT_EQ(parsed.options["option"].as<int>(), 2);
            return 0;
        });

    p.add_option("option", "help");
    subcmd.add_option("option", "help");

    p.parse({"test", "--option", "1", "subcmd", "--option", "2"});
}