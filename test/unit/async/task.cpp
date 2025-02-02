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

TEST(Run, NestedVal) {
    int v = 0;
    ua::run([](int* i) -> ua::Task<void> {
        *i = co_await []() -> ua::Task<int> { co_return 1; }();
    }(&v));
}

TEST(Run, NestedVoid) {
    int v = 0;
    ua::run([](int* i) -> ua::Task<void> {
        co_await [](int* i) -> ua::Task<void> {
            *i = 1;
            co_return;
        }(i);
    }(&v));
}

TEST(Run, NestedMultiple) {
    auto a = [](int v) -> ua::Task<int> { co_return v; };

    int res = 0;
    ua::run([](ua::Task<int> l, ua::Task<int> r, int* out) -> ua::Task<void> {
        *out = co_await l + co_await r;
    }(a(3), a(4), &res));
    ASSERT_EQ(res, 7);
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
