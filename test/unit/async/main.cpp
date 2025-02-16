#include <gtest/gtest.h>

#include <upp/async.hpp>

namespace ua = upp::async;

TEST(Main, Run) {
    auto m = []() -> ua::Main { co_return 1; }();
    ASSERT_EQ(m.run(), 1);
}

TEST(Main, Step) {
    auto m = []() -> ua::Main { co_return 1; }();
    while (!m.done()) m.step();
    ASSERT_EQ(m.result(), 1);
}
