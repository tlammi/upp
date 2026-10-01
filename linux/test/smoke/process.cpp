#include <gtest/gtest.h>

#include <upp/linux/process.hpp>

using upp::linux::process;

TEST(Empty, NotJoinable) {
    auto p = process();
    ASSERT_FALSE(p.joinable());
}

TEST(Fn, NoOp) {
    auto p = process([] {});
    ASSERT_TRUE(p.joinable());
    ASSERT_EQ(p.join(), 0);
}

TEST(Fn, ReturnMaches) {
    auto p = process([] { return 1; });
    ASSERT_EQ(p.join(), 1);
}

TEST(Fn, Args) {
    auto p = process([](int a, int b) { return a + b; }, 1, 2);
    ASSERT_EQ(p.join(), 3);
}
