#include <gtest/gtest.h>

#include <upp/async.hpp>

namespace ua = upp::async;

TEST(Sem, Acquire) {
    [] -> ua::Main {
        auto sem = ua::Semaphore(1);
        EXPECT_EQ(sem.count(), 1);
        co_await sem.acquire();
        EXPECT_EQ(sem.count(), 0);
        co_return 0;
    }()
              .run();
}

TEST(Sem, Release) {
    auto s = ua::Semaphore();
    ASSERT_EQ(s.count(), 0);
    s.release();
    ASSERT_EQ(s.count(), 1);
}

TEST(Sem, TryAcquire) {
    [] -> ua::Main {
        auto sem = ua::Semaphore(0);
        EXPECT_FALSE(sem.try_acquire());
        co_return 0;
    }()
              .run();
}

TEST(Sem, Block) {
    auto sem = ua::Semaphore(0);
    int v = 0;
    [](auto& sem, auto& v) -> ua::Main {
        [](auto& sem, auto& v) -> ua::Task<void> {
            // bunch of yields so the parent is executed first
            co_await ua::yield();
            co_await ua::yield();
            co_await ua::yield();
            co_await ua::yield();
            co_await ua::yield();
            EXPECT_EQ(v, 0);
            ++v;
            sem.release();
            co_return;
        }(sem, v)
                                      .detach();
        co_await sem.acquire();
        EXPECT_EQ(v, 1);
        co_return 0;
    }(sem, v)
                                  .run();
}
