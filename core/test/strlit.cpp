#include <gtest/gtest.h>

#include <upp/strlit.hpp>

using namespace upp::literals;
using namespace std::literals;

TEST(Ctor, Default) {
    auto s = upp::strlit();
    ASSERT_TRUE(s.empty());
}

TEST(Ctor, Value) {
    auto s = upp::strlit("foobar");
    ASSERT_EQ(s, "foobar");
}

TEST(Literal, Simple) {
    auto s = "foobar"_lit;
    ASSERT_EQ(s, "foobar");
}
