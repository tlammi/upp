#include <gtest/gtest.h>

#include <upp/async.hpp>

namespace ua = upp::async;

TEST(Yield, Flat) {
    size_t counter = 0;
    auto m = [](auto& counter) -> ua::Main {
        ++counter;
        co_await ua::yield();
        ++counter;
        co_await ua::yield();
        ++counter;
        co_await ua::yield();
        ++counter;
        co_return 0;
    }(counter);
    while (counter == 0) { m.step(); }
    ASSERT_EQ(counter, 1);
    m.step();
    ASSERT_EQ(counter, 2);
    m.step();
    ASSERT_EQ(counter, 3);
    m.step();
    ASSERT_EQ(counter, 4);
    while (!m.done()) m.step();
    ASSERT_EQ(m.result(), 0);
}

TEST(Yield, Nested) {
    size_t counter = 0;
    auto t = [](auto& counter) -> ua::Task<void> {
        ++counter;
        co_await ua::yield();
        ++counter;
    };

    auto m = [](auto& counter, auto& mk_task) -> ua::Main {
        co_await mk_task(counter);
        co_await mk_task(counter);
        co_return 0;
    }(counter, t);

    auto loop_until = [&](size_t expected) {
        while (counter < expected) m.step();
        EXPECT_EQ(counter, expected);
    };
    loop_until(1);
    loop_until(2);
    loop_until(3);
    loop_until(4);
    while (!m.done()) m.step();
}

