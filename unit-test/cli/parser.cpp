#include "upp/cli/parser.hpp"
#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ::testing;

TEST(ParserTest, BoolOpts) {
    upp::cli::Parser<int> p;
    p.boolopts().add('h', "help");
    p.boolopts()['h'] = true;
    ASSERT_TRUE(p.boolopts()["help"]);
}

TEST(ParserTest, Opts) {
    upp::cli::Parser<int> p;
    p.opts().add('d', "demo");
    p.opts()['d'] = "hello, world";
    ASSERT_EQ(p.opts()["demo"].as<std::string>(), "hello, world");
    ASSERT_EQ(p.opts()['d'].as<std::string>(),
              p.opts()["demo"].as<const char*>());
}

TEST(ParserTest, VectOpts) {
    upp::cli::Parser<int> p;
    p.vectopts().add('v', "vect-opt");
    p.vectopts()['v'].push_back("169");
    p.vectopts()['v'].push_back("168");
    auto intvect = p.vectopts()["vect-opt"].as<int>();
    ASSERT_THAT(intvect, ElementsAreArray({169, 168}));
}

TEST(ParserTest, Parse) {
    upp::cli::Parser<int> p;
    const char* argv[] = {"demo", "-f",    "-o",     "option",
                          "-v",   "value", "--vect", "value"};

    p.boolopts().add('f', "flag");
    p.opts().add('o', "option");
    p.vectopts().add('v', "vect");
    p.parse(sizeof(argv) / sizeof(argv[0]), argv);
    ASSERT_TRUE(p.boolopts()["flag"]);
    ASSERT_EQ(p.opts()['o'].as<std::string>(), "option");
    ASSERT_THAT(p.vectopts()["vect"].as<std::string>(),
                UnorderedElementsAre("value", "value"));
}

TEST(ParserTest, Flag) {
    upp::cli::Parser<int> p;
    p.boolopts().add('f', "flag");
    p.parse(std::vector<std::string>({"--flag"}));
    ASSERT_TRUE(p.boolopts()['f']);
    p.parse(std::vector<std::string>({}));
    ASSERT_FALSE(p.boolopts()["flag"]);
}

TEST(ParserTest, Option) {
    upp::cli::Parser<int> p;
    p.opts().add('o', "opt");
    p.parse(std::vector<std::string>({"-o", "101"}));
    ASSERT_EQ(p.opts()["opt"].as<int>(), 101);
    p.parse(std::vector<std::string>({"-o", "102"}));
    ASSERT_EQ(p.opts()['o'].as<short>(), 102);
    p.parse(std::vector<std::string>({}));
    ASSERT_EQ(p.opts()['o'].as<std::string>(), "");
}

TEST(ParserTest, VectOption) {
    upp::cli::Parser<short> p;
    p.vectopts().add('v', "vect");
    p.parse(std::vector<std::string>({"-v", "0", "--vect", "1", "-v", "2"}));
    ASSERT_THAT(p.vectopts()["vect"].as<int>(), UnorderedElementsAre(0, 1, 2));
    p.parse(std::vector<std::string>());
    ASSERT_EQ(p.vectopts()['v'].size(), 0);
}

TEST(ParserTest, PosArgs) {
    upp::cli::Parser<int> p;
    ASSERT_EQ(p.pos_args().size(), 0);
    p.parse(std::vector<std::string>({"arg0", "arg1", "arg2"}));
    ASSERT_THAT(p.pos_args().as<std::string>(),
                ElementsAre("arg0", "arg1", "arg2"));
}

TEST(ParserTest, Subparser) {
    upp::cli::Parser<int> p;
    p.subcommands()["subcmd"] = upp::cli::Parser<int>();
    p.parse(std::vector<std::string>({"subcmd", "arg0", "arg1"}));
    ASSERT_EQ(p.parsed_subcmd(), "subcmd");
    auto tmp{p.subcommands()["subcmd"].pos_args().as<std::string>()};
    ASSERT_THAT(tmp, ElementsAre("arg0", "arg1"));
}