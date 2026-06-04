#include <gtest/gtest.h>

#include <ranges>
#include <upp/intrusive_list.hpp>

class node {
    friend upp::intrusive_list_traits<node>;
    node* m_next;
    node*& next() noexcept { return m_next; }

 public:
    constexpr node() noexcept = default;

    node(const node&) noexcept = default;
    node& operator=(const node&) noexcept = default;

    node(node&&) = delete;
    node& operator=(node&&) = delete;

    constexpr ~node() = default;

    // Separate method to test that next() is accessible from the library
    node* public_next() const noexcept { return m_next; }
};

using list = upp::intrusive_list<node>;

TEST(Ctor, Default) {
    auto l = list();
    ASSERT_TRUE(l.empty());
    ASSERT_EQ(l.size(), 0);
}

TEST(Access, PushPop) {
    auto n = node();
    auto l = list();
    l.push_back(n);
    ASSERT_EQ(l.size(), 1);
    ASSERT_FALSE(l.empty());
    ASSERT_EQ(&l.front(), &n);
    ASSERT_EQ(&l.back(), &n);
    l.pop_front();
    ASSERT_EQ(l.size(), 0);
    ASSERT_TRUE(l.empty());
}

TEST(Access, PushPop2) {
    auto n1 = node();
    auto n2 = node();
    auto l = list();
    l.push_back(n1);
    l.push_back(n2);
    ASSERT_EQ(l.size(), 2);
    ASSERT_EQ(&l.front(), &n1);
    ASSERT_EQ(&l.back(), &n2);
    ASSERT_FALSE(l.empty());
    l.pop_front();
    ASSERT_EQ(l.size(), 1);
    ASSERT_EQ(&l.front(), &n2);
    ASSERT_EQ(&l.back(), &n2);
    ASSERT_FALSE(l.empty());
    l.pop_front();
    ASSERT_EQ(l.size(), 0);
    ASSERT_TRUE(l.empty());
}

TEST(Iterate, Empty) {
    auto l = list();
    auto v = l | std::ranges::to<std::vector>();
    ASSERT_TRUE(v.empty());
}

TEST(Iterate, Values) {
    auto n1 = node();
    auto n2 = node();
    auto n3 = node();

    auto l = list(std::in_place, n1, n2, n3);
    auto v = l | std::ranges::to<std::vector>();
    ASSERT_EQ(v.size(), 3);
}
