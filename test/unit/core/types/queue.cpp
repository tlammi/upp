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
    auto q = upp::Queue{1, 2, 3, 4};
    ASSERT_FALSE(q.empty());
    ASSERT_EQ(q.size(), 4);
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

TEST(Iterate, CreateVectorConst) {
    const auto q = upp::Queue<int>{1, 2, 3};
    auto v = std::vector<int>(q.begin(), q.end());
    ASSERT_THAT(v, ElementsAre(1, 2, 3));
}

TEST(Add, EmplaceBack) {
    // large array to fill the storage faster
    using Elem = std::array<uint8_t, 100>;  // NOLINT
    auto q = upp::Queue<Elem>();
    static constexpr auto count = 100;
    for (size_t i = 0; i < count; ++i) {
        ASSERT_EQ(q.size(), i);
        q.emplace_back();
    }
    ASSERT_EQ(q.size(), count);
}

TEST(Add, EmplaceFront) {
    using Elem = std::array<uint8_t, 100>;  // NOLINT
    auto q = upp::Queue<Elem>();
    static constexpr auto count = 100;
    for (size_t i = 0; i < count; ++i) {
        ASSERT_EQ(q.size(), i);
        q.emplace_front();
    }
}
TEST(Rm, PopBackAll) {
    using Elem = std::array<uint8_t, 100>;
    auto q = upp::Queue<Elem>();
    static constexpr size_t count = 100;
    for (size_t i = 0; i < count; ++i) { q.emplace_back(); }
    for (size_t i = count; i > 0; --i) {
        ASSERT_EQ(q.size(), i);
        q.pop_back();
    }
}

TEST(Rm, PopFrontAll) {
    using Elem = std::array<uint8_t, 100>;
    auto q = upp::Queue<Elem>();
    static constexpr size_t count = 100;
    for (size_t i = 0; i < count; ++i) { q.emplace_front(); }
    for (size_t i = count; i > 0; --i) {
        ASSERT_EQ(q.size(), i);
        q.pop_front();
    }
}
