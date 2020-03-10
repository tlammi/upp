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