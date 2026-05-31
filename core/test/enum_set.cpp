#include <gtest/gtest.h>

#include <upp/enum_set.hpp>

enum class sample {
    a,
    b,
    c,
    d,
    e,
};

TEST(Ctor, Default) {
    auto s = upp::enum_set<sample>();
    ASSERT_FALSE(s);
}

TEST(Ctor, Values) {
    using enum sample;
    auto s = upp::enum_set<sample>{a, b, c};
    ASSERT_TRUE(s);
    ASSERT_TRUE(s.contains(a));
    ASSERT_TRUE(s.contains(b));
    ASSERT_TRUE(s.contains(c));
    ASSERT_FALSE(s.contains(d));
    ASSERT_FALSE(s.contains(e));
}
