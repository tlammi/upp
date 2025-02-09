#include <gtest/gtest.h>

#include <upp/async.hpp>

namespace ua = upp::async;

TEST(Mut, Ctor) { ua::Mutex(); }

TEST(Mut, LockUnlock) {
    ua::Mutex mut{};
    ua::run([](auto& mut) -> ua::Task<void> {
        co_await mut.lock();
        EXPECT_TRUE(mut.locked());
        mut.unlock();
        EXPECT_FALSE(mut.locked());
    }(mut));
}

TEST(Mut, LockObject) {
    ua::Mutex mut{};
    ua::run([](auto& mut) -> ua::Task<void> {
        EXPECT_FALSE(mut.locked());
        {
            auto lk = co_await ua::unique_lock(mut);
            EXPECT_TRUE(mut.locked());
        }
        EXPECT_FALSE(mut.locked());
    }(mut));
}

TEST(Mut, Guard) {
    ua::Mutex mut{};
    int val{};

    auto a = [](auto& mut, auto& val) -> ua::Task<void> {
        {
            auto lk = co_await ua::unique_lock(mut);
            val += 1;
            co_await ua::yield();
            val += 1;
            co_await ua::yield();
            val += 1;
        }
        co_await ua::yield();
        co_await ua::yield();
        co_await ua::yield();
        co_await ua::yield();
        co_await ua::yield();
        val += 1;
    }(mut, val);

    auto b = [](auto& mut, auto& val) -> ua::Task<void> {
        auto lk = co_await ua::unique_lock(mut);
        val *= 2;
        co_await ua::yield();
        co_await ua::yield();
    }(mut, val);

    ua::run([](auto& a, auto& b) -> ua::Task<void> {
        co_await +std::move(a);
        co_await +std::move(b);
        co_return;
    }(a, b));

    ASSERT_EQ(val, 7);
}

