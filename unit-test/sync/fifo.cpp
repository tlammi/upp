#include "upp/sync/fifo.hpp"
#include <gtest/gtest.h>
#include "upp/sync/thread.hpp"

TEST(FifoTest, TryPush) {
    upp::sync::Fifo<int, 5> f;
    for (size_t i = 0; i < 5; ++i) { ASSERT_TRUE(f.try_push(i)); }
    ASSERT_FALSE(f.try_push(100));
}

TEST(FifoTest, Capacity) {
    upp::sync::Fifo<int, 3> f;
    ASSERT_EQ(f.capacity(), 3);
}

TEST(FifoTest, Count) {
    upp::sync::Fifo<int, 3> f;
    ASSERT_EQ(f.count(), 0);
    for (size_t i = 0; i < 3; ++i) {
        f.push(i);
        ASSERT_EQ(f.count(), i + 1);
    }
    f.push(69);
    ASSERT_EQ(f.count(), 3);
    f.push(96);
    ASSERT_EQ(f.count(), 3);

    for (size_t i = 3; i > 0; --i) {
        f.try_pop();
        ASSERT_EQ(f.count(), i - 1);
    }
}

TEST(FifoTest, TryPop1) {
    upp::sync::Fifo<int, 4> f;
    for (size_t i = 0; i < 4; ++i) { ASSERT_TRUE(f.try_push(i)); }
    for (size_t i = 0; i < 4; ++i) {
        auto out = f.try_pop();
        ASSERT_TRUE(out);
        ASSERT_EQ(*out, i);
    }
}

TEST(FifoTest, TryPop2) {
    upp::sync::Fifo<int, 3> f;
    for (size_t i = 0; i < 100; i++) {
        f.push(i);
        auto out = f.try_pop();
        ASSERT_TRUE(out);
        ASSERT_EQ(*out, i);
    }
}

TEST(FifoTest, SizeOf1) {
    upp::sync::Fifo<int, 1> f;
    ASSERT_TRUE(f.try_push(100));
    ASSERT_FALSE(f.try_push(101));
    auto out = f.try_pop();
    ASSERT_TRUE(out);
    ASSERT_EQ(*out, 100);
    out = f.try_pop();
    ASSERT_FALSE(out);
    out = f.try_pop();
    ASSERT_FALSE(out);
    f.push(10);
    f.push(20);
    ASSERT_EQ(f.pop(), 20);
}

TEST(FifoTest, Thread1) {
    upp::sync::Fifo<int, 10> f;
    volatile bool start_flag{false};
    volatile bool stop_flag{false};
    upp::sync::Thread t{[&]() {
        start_flag = true;
        ASSERT_EQ(f.pop(), 100);
        stop_flag = true;
        f.push(10);
        f.push(20);
    }};

    while (!start_flag) {}
    f.push(100);
    while (!stop_flag) {}
    ASSERT_EQ(f.pop(), 10);
    ASSERT_EQ(f.pop(), 20);
    t.join();
}

TEST(FifoTest, Thread2) {
    upp::sync::Fifo<int, 10> f;
    upp::sync::Thread t{[&]() {
        for (size_t i = 0; i < 10; ++i) ASSERT_EQ(f.pop(), i);
    }};

    for (size_t i = 0; i < 10; ++i) f.push(i);
    t.join();
}

TEST(FifoTest, Thread3) {
    upp::sync::Fifo<int, 2> f;
    volatile bool flag{false};
    f.push(2);
    f.push(3);
    f.push(100);
    upp::sync::Thread t{[&]() {
        ASSERT_EQ(f.pop(), 3);
        ASSERT_EQ(f.pop(), 100);
        flag = true;
        ASSERT_EQ(f.pop(), 6);
    }};
    while (!flag) {}
    f.push(6);
    t.join();
}