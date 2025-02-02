#include <gtest/gtest.h>

#include <upp/async.hpp>

namespace ua = upp::async;

TEST(Ctor, ReturnVoid) {
    []() -> ua::Task<> { co_return; }();
}

TEST(Ctor, ReturnVal) {
    []() -> ua::Task<int> { co_return 1; }();
}

TEST(Run, Simple) {
    int v = 0;
    ua::run([](int* i) -> ua::Task<void> {
        ++(*i);
        co_return;
    }(&v));
    ASSERT_EQ(v, 1);
}

TEST(Run, Detach) {
    int v = 0;
    ua::run([](int* i) -> ua::Task<void> {
        co_await +[](int* i) -> ua::Task<void> {
            ++*i;
            co_return;
        }(i);

        co_return;
    }(&v));
    ASSERT_EQ(v, 1);
}

/*
TEST(Run, Yield) {
    int v = 0;
    auto sched = ua::Scheduler();
    sched.dispatch([](int* i) -> ua::Task<void> {
        ++*i;
        co_await ua::yield();
        ++*i;
        co_await ua::yield();
        ++*i;
    }(&v));
    while (v == 0) sched.step();
    ASSERT_EQ(v, 1);
    while (v == 1) sched.step();
    ASSERT_EQ(v, 2);
    while (!sched.done()) sched.step();
    ASSERT_EQ(v, 2);
}
*/
