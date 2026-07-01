#include <gtest/gtest.h>

#include <upp/lockfree/intrusive_stack.hpp>

namespace lf = upp::lockfree;

struct node {
    std::atomic<node*> next{};
};

template <>
struct upp::lockfree::intrusive_node_traits<node> {
    static constexpr std::atomic<node*>& next(node* curr) noexcept {
        return curr->next;
    }
};

TEST(Simple, Push) {
    auto s = lf::intrusive_stack<node>();
    auto n = node();
    s.push(n);
    auto e = s.extract();
    ASSERT_EQ(e.size(), 1);
    ASSERT_TRUE(s.empty());
}

TEST(Simple, PushMultiple) {
    auto s = lf::intrusive_stack<node>();
    auto n0 = node();
    auto n1 = node();
    auto n2 = node();
    auto n3 = node();

    s.push(n0);
    s.push(n1);
    s.push(n2);
    s.push(n3);

    auto e = s.extract();
    ASSERT_EQ(e.size(), 4);
}
