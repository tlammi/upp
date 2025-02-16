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
