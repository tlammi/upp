#include <gtest/gtest.h>

#include <upp/ring_buffer.hpp>

TEST(Ctor, Default) {
    auto b = upp::ring_buffer<int>();
    ASSERT_TRUE(b.empty());
    ASSERT_EQ(b.size(), 0);
    ASSERT_EQ(b.capacity(), 0);
}

TEST(Ctor, Capacity) {
    static constexpr auto cap = 10u;
    auto b = upp::ring_buffer<int>(cap);
    ASSERT_TRUE(b.empty());
    ASSERT_EQ(b.size(), 0);
    ASSERT_EQ(b.capacity(), cap);
}

TEST(PushPop, Trivial) {
    auto b = upp::ring_buffer<int>(1);
    b.push(2);
    ASSERT_EQ(b.size(), 1);
    ASSERT_EQ(b.front(), 2);
    b.pop();
    ASSERT_TRUE(b.empty());
}
