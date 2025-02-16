#include <gtest/gtest.h>

#include <upp/async.hpp>

namespace ua = upp::async;

TEST(Mut, Ctor) { ua::Mutex(); }

TEST(Mut, LockUnlock) {
    ua::Mutex mut{};
    [](auto* mut) -> ua::Main {
        co_await mut->lock();
        EXPECT_TRUE(mut->locked());
        mut->unlock();
        EXPECT_FALSE(mut->locked());
        co_return 0;
    }(&mut)
                         .run();
}

TEST(Mut, LockObject) {
    ua::Mutex mut{};
    [](auto* mut) -> ua::Main {
        EXPECT_FALSE(mut->locked());
        {
            auto lk = co_await ua::unique_lock(*mut);
            EXPECT_TRUE(mut->locked());
        }
        EXPECT_FALSE(mut->locked());
        co_return 0;
    }(&mut)
                         .run();
}

TEST(Mut, Guard) {
    ua::Mutex mut{};
    int val{};

    auto a = [](auto* mut, auto* val) -> ua::Task<> {
        {
            auto lk = co_await ua::unique_lock(*mut);
            *val += 1;
            co_await ua::yield();
            *val += 1;
            co_await ua::yield();
            *val += 1;
        }
        co_await ua::yield();
        co_await ua::yield();
        co_await ua::yield();
        co_await ua::yield();
        co_await ua::yield();
        *val += 1;
    }(&mut, &val);

    auto b = [](auto* mut, auto* val) -> ua::Task<void> {
        auto lk = co_await ua::unique_lock(*mut);
        *val *= 2;
        co_await ua::yield();
        co_await ua::yield();
    }(&mut, &val);

    [](auto* a, auto* b) -> ua::Main {
        +std::move(*a);
        +std::move(*b);
        co_return 0;
    }(&a, &b)
                                .run();

    ASSERT_EQ(val, 7);
}
