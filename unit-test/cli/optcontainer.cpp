#include "upp/cli/optcontainer.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace upp::cli;
using namespace ::testing;

TEST(OptContainerTest, BoolContainer) {
    OptContainer<bool> a;
    a.add('a', "A");
    a.add('b', "B");
    ASSERT_FALSE(a['a']);
    ASSERT_FALSE(a['b']);
    a['a'] = true;
    a["B"] = true;
    ASSERT_TRUE(a['a']);
    ASSERT_TRUE(a['b']);
    a.reset_values();
    ASSERT_FALSE(a['a']);
    ASSERT_FALSE(a["B"]);
}

TEST(OptContainerTest, StrContainer) {
    OptContainer<std::string> c;
    c.add('a', "A");
    c.add('b', "B");
    ASSERT_EQ(c['a'], "");
    ASSERT_EQ(c['b'], "");
    c['a'] = "hello";
    c['b'] = "world";
    ASSERT_EQ(c['a'], "hello");
    ASSERT_EQ(c['b'], "world");
    c.reset_values();
    ASSERT_EQ(c['a'], "");
    ASSERT_EQ(c['b'], "");
}

TEST(OptContainerTest, Flags) {
    OptContainer<int> c;
    c.add('a', "A");
    c.add("B");
    ASSERT_THAT(c.shortflags(), ElementsAreArray({'a'}));
    ASSERT_THAT(c.longflags(), UnorderedElementsAre("A", "B"));
}

TEST(OptContainerTest, ConstIndex) {
    OptContainer<long> c;
    c.add('a', "A");
    c["A"] = 1000;
    const auto& r = c;
    ASSERT_EQ(r["A"], 1000);
    ASSERT_EQ(r['a'], 1000);
}