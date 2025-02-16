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
    [] -> ua::Main {
        auto channel = ua::Channel<int>();
        std::println("a");
        +[](auto* chan) -> ua::Task<void> {
            std::println("b");
            chan->write(1);
            std::println("c");
            co_return;
        }(&channel);
        std::println("d");
        auto res = co_await channel.read();
        std::println("e");
        EXPECT_EQ(res, 1);
        std::println("f");
        co_return 0;
    }()
              .run();
}

TEST(Produce, Void) {
    [] -> ua::Main {
        auto channel = ua::Channel<void>();
        +[](auto* chan) -> ua::Task<void> {
            chan->write();
            co_return;
        }(&channel);
        co_await channel.read();
        EXPECT_EQ(channel.size(), 0);
        co_return 0;
    }()
              .run();
}
