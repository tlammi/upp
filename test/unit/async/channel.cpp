#include <gtest/gtest.h>

#include <upp/async.hpp>

namespace ua = upp::async;

TEST(Ctor, Void) {
    auto c = ua::Channel<void>();
    ASSERT_TRUE(c.empty());
}

TEST(Ctor, Int) {
    auto c = ua::Channel<int>();
    ASSERT_TRUE(c.empty());
}

TEST(Produce, Int) {
    ua::run([] -> ua::Task<void> {
        auto channel = ua::Channel<int>();
        co_await +[](auto& chan) -> ua::Task<void> {
            chan.write(1);
            co_return;
        }(channel);
        auto res = co_await channel.read();
        EXPECT_EQ(res, 1);
    }());
}

TEST(Produce, Void) {
    ua::run([] -> ua::Task<void> {
        auto channel = ua::Channel<void>();
        co_await +[](auto& chan) -> ua::Task<void> {
            chan.write();
            co_return;
        }(channel);
        co_await channel.read();
        EXPECT_EQ(channel.size(), 0);
    }());
}
