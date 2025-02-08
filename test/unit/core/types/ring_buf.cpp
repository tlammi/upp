#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <upp/types.hpp>

static_assert(std::random_access_iterator<upp::RingBuf<int>::iterator>);

template <class T>
using RingBuf = upp::RingBuf<T>;

using testing::ElementsAre;

constexpr auto mk_simple_buf() { return upp::RingBuf<int>{1, 2, 3, 4}; }

TEST(Ctor, Default) {
    auto b = RingBuf<int>();
    ASSERT_TRUE(b.empty());
    ASSERT_EQ(b.size(), 0);
}

TEST(Ctor, InitializerList) {
    auto b = mk_simple_buf();
    ASSERT_FALSE(b.empty());
    ASSERT_EQ(b.size(), 4);
}

TEST(Ctor, Move) {
    auto a = mk_simple_buf();
    auto b = std::move(a);
    ASSERT_THAT(b, ElementsAre(1, 2, 3, 4));
}

TEST(Assign, Move) {
    auto a = mk_simple_buf();
    a = upp::RingBuf{1};
    ASSERT_THAT(a, ElementsAre(1));
}

TEST(Comparison, Equal) {
    auto a = mk_simple_buf();
    auto b = a;
    ASSERT_EQ(a, b);
}

TEST(Comparison, NotEqual) {
    auto a = upp::RingBuf{1};
    auto b = upp::RingBuf{2};
    ASSERT_NE(a, b);
}

TEST(Comparison, Shorter) {
    auto a = mk_simple_buf();
    auto b = a;
    b.push_back(1);
    ASSERT_NE(a, b);
    ASSERT_LT(a, b);
}

TEST(Iterate, CreateVec) {
    auto b = mk_simple_buf();
    auto v = std::vector(b.begin(), b.end());

    ASSERT_THAT(v, ElementsAre(1, 2, 3, 4));
}

TEST(Iterate, Const) {
    const auto b = mk_simple_buf();
    ASSERT_THAT(b, ElementsAre(1, 2, 3, 4));
}

TEST(MemberAccess, First) {
    auto b = mk_simple_buf();
    ASSERT_EQ(b[0], 1);
    ASSERT_EQ(b.at(0), 1);
    ASSERT_EQ(b.front(), 1);
}

TEST(MemberAccess, Last) {
    auto b = mk_simple_buf();
    ASSERT_EQ(b[b.size() - 1], 4);
    ASSERT_EQ(b.at(b.size() - 1), 4);
    ASSERT_EQ(b.back(), 4);
}

TEST(AddItems, EmplaceBack) {
    auto b = upp::RingBuf<int>();
    b.emplace_back(1);
    ASSERT_THAT(b, ElementsAre(1));
}

TEST(AddItems, EmplaceBackMany) {
    auto b = upp::RingBuf<int>();
    static constexpr size_t count = 10000;
    for (size_t i = 0; i < count; ++i) { b.emplace_back(i); }
    ASSERT_EQ(b.size(), count);
    ASSERT_GT(b.capacity(), count);
    for (size_t i = 0; i < count; ++i) { ASSERT_EQ(b[i], i); }
}

TEST(AddItems, EmplaceFront) {
    auto b = upp::RingBuf<int>();
    b.emplace_front(1);
    ASSERT_THAT(b, ElementsAre(1));
}

TEST(AddItems, EmplaceFrontMany) {
    auto b = upp::RingBuf<int>();
    static constexpr size_t count = 10000;
    for (size_t i = 0; i < count; ++i) { b.emplace_front(i); }
    ASSERT_EQ(b.size(), count);
    ASSERT_GT(b.capacity(), count);
    for (size_t i = 0; i < count; ++i) { ASSERT_EQ(b[i], count - i - 1); }
}

TEST(Capacity, Reserve) {
    static constexpr size_t count = 100;
    auto b = upp::RingBuf<int>();
    b.reserve(count);
    ASSERT_GE(b.capacity(), count);
}

TEST(ResourceManagement, Default) {
    upp::RingBuf<std::unique_ptr<int>>(std::in_place, new int(1), new int(2),
                                       new int(3));
}
