#include <gtest/gtest.h>

#include <print>
#include <upp/async.hpp>

namespace ua = upp::async;

TEST(Await, Val) {
    auto t = []() -> ua::Task<int> { co_return 1; };
    auto m = [](auto t) -> ua::Main { co_return co_await t(); }(std::move(t));
    ASSERT_EQ(m.run(), 1);
}

TEST(Await, Void) {
    int res = 0;
    auto t = [](int* i) -> ua::Task<> {
        std::println("t");
        *i = 1;
        std::println("t2");
        co_return;
    };
    auto m = [](auto t) -> ua::Main {
        co_await t;
        co_return 0;
    }(t(&res));
    m.run();
    ASSERT_EQ(res, 1);
}

TEST(Await, ValNested) {
    auto m = []() -> ua::Main {
        co_return co_await []() -> ua::Task<int> {
            co_return co_await []() -> ua::Task<int> { co_return 1; }();
        }();
    }();

    ASSERT_EQ(m.run(), 1);
}

TEST(Detach, One) {
    int counter = 0;
    auto t = [](int* counter) -> ua::Task<> {
        ++*counter;
        co_return;
    }(&counter);
    auto m = [](auto t) -> ua::Main {
        +std::move(t);
        co_return 0;
    }(std::move(t));
    m.run();
    ASSERT_EQ(counter, 1);
}

TEST(Detach, Two) {
    int counter = 0;
    auto t = [](int* counter) -> ua::Task<> {
        ++*counter;
        co_return;
    };

    auto m = [](auto a, auto b) -> ua::Main {
        +std::move(a);
        +std::move(b);
        co_return 0;
    }(t(&counter), t(&counter));
    m.run();
    ASSERT_EQ(counter, 2);
}

#if 0
TEST(Ctor, ReturnVoid) {
    []() -> ua::Task<> { co_return; }().extract();
}

TEST(Ctor, ReturnVal) {
    []() -> ua::Task<int> { co_return 1; }().extract();
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
    ASSERT_EQ(v, 3);
}
#endif
