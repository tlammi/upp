#include "upp/cli/parser.hpp"
#include <gtest/gtest.h>
TEST(ParserTest, BoolOpts) {
    upp::cli::Parser p;
    p.boolopts().add('h', "help");
    p.boolopts()['h'] = true;
    ASSERT_TRUE(p.boolopts()["help"]);
}

TEST(ParserTest, Opts) {
    upp::cli::Parser p;
    p.opts().add('d', "demo");
    p.opts()['d'] = "hello, world";
    ASSERT_EQ(p.opts()["demo"].as<std::string>(), "hello, world");
    ASSERT_EQ(p.opts()['d'].as<std::string>(),
              p.opts()["demo"].as<const char*>());
}

TEST(ParserTest, VectOpts) {
    upp::cli::Parser p;
    p.vectopts().add('v', "vect-opt");
    p.vectopts()['v'].push_back("169");
    auto intvect = p.vectopts()["vect-opt"].as<int>();
    std::vector<int> expected{169};
    ASSERT_EQ(intvect.size(), expected.size());
    for (size_t i = 0; i < intvect.size(); ++i) {
        ASSERT_EQ(intvect[i], expected[i]);
    }
}