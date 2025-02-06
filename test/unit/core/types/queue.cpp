#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <upp/types.hpp>

using testing::ElementsAre;

TEST(Ctor, Default) {
    auto q = upp::Queue<int>();
    ASSERT_TRUE(q.empty());
    ASSERT_EQ(q.size(), 0);
    ASSERT_EQ(q.length(), 0);
}

TEST(Ctor, InitializerList) {
    auto q = upp::Queue{1, 2, 3, 4, 5};
    ASSERT_FALSE(q.empty());
    ASSERT_EQ(q.size(), 5);
}

TEST(Iterate, Empty) {
    auto q = upp::Queue<int>();
    ASSERT_EQ(q.begin(), q.end());
}

TEST(Iterate, Items) {
    auto q = upp::Queue<int>{1, 2, 3};
    auto iter = q.begin();
    ASSERT_EQ(*iter, 1);
    ++iter;
    ASSERT_EQ(*iter, 2);
    ++iter;
    ASSERT_EQ(*iter, 3);
    ++iter;
    ASSERT_EQ(iter, q.end());
}

TEST(Iterate, ForLoop) {
    auto q = upp::Queue<int>{1, 2, 3};
    std::vector<int> out{};
    for (auto&& v : q) { out.push_back(std::move(v)); }
    ASSERT_THAT(out, ElementsAre(1, 2, 3));
}

TEST(Iterate, CreateVector) {
    auto q = upp::Queue<int>{1, 2, 3};
    auto v = std::vector<int>(q.begin(), q.end());
    ASSERT_THAT(v, ElementsAre(1, 2, 3));
}
