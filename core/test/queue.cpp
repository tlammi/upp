#include <gtest/gtest.h>

#include <upp/queue.hpp>

template <class T>
using queue = upp::experimental::queue<T>;

TEST(Ctor, Default) {
    auto q = queue<int>();
    ASSERT_TRUE(q.empty());
    ASSERT_EQ(q.size(), 0);
}

TEST(Ctor, InitializerList) {
    auto q = queue<int>{1, 2, 3, 4};
    ASSERT_FALSE(q.empty());
    ASSERT_EQ(q.size(), 4);
}

TEST(Push, ToEmpty) {
    auto q = queue<int>();
    q.push(1);
    ASSERT_FALSE(q.empty());
    ASSERT_EQ(q.size(), 1);
}

TEST(Push, Many) {
    static constexpr auto count = 10000u;
    auto q = queue<unsigned>();
    for (auto i = 0u; i < count; ++i) { q.push(i); }
    ASSERT_FALSE(q.empty());
    ASSERT_EQ(q.size(), count);
}

TEST(Pop, OneSize) {
    auto q = queue<int>{1};
    q.pop();
    ASSERT_TRUE(q.empty());
    ASSERT_EQ(q.size(), 0);
}
