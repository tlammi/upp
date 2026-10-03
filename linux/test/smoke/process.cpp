#include <gtest/gtest.h>

#include <upp/linux/process.hpp>

using upp::linux::process;

using namespace std::literals;

TEST(Empty, NotJoinable) {
    auto p = process();
    ASSERT_FALSE(p.joinable());
}

TEST(Fn, NoOp) {
    auto p = process([] {});
    ASSERT_TRUE(p.joinable());
    auto res = p.join();
    ASSERT_TRUE(res.ok());
    ASSERT_EQ(res.exit_code, 0);
    ASSERT_EQ(res.signal, 0);
}

TEST(Fn, ReturnMaches) {
    auto p = process([] { return 1; });
    auto res = p.join();
    ASSERT_EQ(res.exit_code, 1);
    ASSERT_EQ(res.signal, 0);
}

TEST(Fn, Args) {
    auto p = process([](int a, int b) { return a + b; }, 1, 2);
    ASSERT_EQ(p.join().exit_code, 3);
}

TEST(Fn, Kill) {
    auto p = process([] { pause(); });
    p.kill(SIGINT);
    ASSERT_EQ(p.join().signal, SIGINT);
}
