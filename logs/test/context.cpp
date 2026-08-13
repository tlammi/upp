#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <upp/unused.hpp>

#include "contextutils.hpp"

namespace ul = upp::logs;

using testing::UnorderedElementsAre;

struct test_ctx {
    test_ctx* _next{};
    test_ctx* _prev{};
    test_ctx* _parent{};

    constexpr test_ctx() noexcept = default;
    constexpr test_ctx(test_ctx& root, test_ctx& parent) noexcept
        : _next(root._next), _prev(&root), _parent(&parent) {
        if (_next) _next->_prev = this;
        root._next = this;
    }

    test_ctx(const test_ctx&) = delete;
    test_ctx& operator=(const test_ctx&) = delete;

    test_ctx(test_ctx&&) = delete;
    test_ctx& operator=(test_ctx&&) = delete;

    constexpr ~test_ctx() = default;

    constexpr test_ctx* next() const noexcept { return _next; }
    constexpr test_ctx* prev() const noexcept { return _prev; }
    constexpr test_ctx* parent() const noexcept { return _parent; }

    constexpr void set_next(int, test_ctx* c) noexcept { _next = c; }
    constexpr void set_prev(int, test_ctx* c) noexcept { _prev = c; }
};

bool has_grandparent(const test_ctx* ctx, const test_ctx* gp) noexcept {
    for (auto* p = ctx->parent(); p != nullptr; p = p->parent()) {
        if (p == gp) return true;
    }
    return false;
}

std::vector<test_ctx*> all_children_of(test_ctx* root,
                                       test_ctx* parent) noexcept {
    auto out = std::vector<test_ctx*>();
    auto* i = root;
    while (i) {
        if (i->parent() == parent) out.push_back(i);
        i = i->next();
    }
    return out;
}

// List children in their correct positions
std::vector<test_ctx*> children_of(test_ctx* parent) noexcept {
    std::vector<test_ctx*> out{};
    for (auto* i = parent->next(); i != nullptr && has_grandparent(i, parent);
         i = i->next()) {
        if (i->parent() == parent) { out.push_back(i); }
    }
    return out;
}

constexpr bool is_organized(const test_ctx* root) noexcept {
    auto* parent = root;
    for (const auto* i = parent->next(); i != nullptr; i = i->next()) {
        if (i->parent() == parent) continue;
        if (i->parent() == i->prev()) parent = i->prev();
    }
    return true;
}

TEST(OrganizeNext, One) {
    auto root = test_ctx();
    auto child = test_ctx{root, root};
    auto* pivot = root.next();
    auto* curr = pivot;
    ul::organize_next_child(&root, pivot, curr);
    ASSERT_EQ(pivot, nullptr);
    ASSERT_EQ(curr, nullptr);
    ASSERT_EQ(root._next, &child);
    ASSERT_EQ(root._prev, nullptr);
    ASSERT_EQ(child._next, nullptr);
    ASSERT_EQ(child._prev, &root);
}

TEST(OrganizeNext, Multiple) {
    auto root = test_ctx();
    auto a = test_ctx(root, root);
    auto b = test_ctx(root, root);
    auto c = test_ctx(root, root);

    auto* pivot = root.next();
    auto* orig = pivot;
    auto* curr = pivot;

    ul::organize_next_child(&root, pivot, curr);
    ASSERT_EQ(root.next(), orig);
}

TEST(OrganizeNext, WithGrandchildren) {
    auto root = test_ctx();
    auto a = test_ctx(root, root);
    auto b = test_ctx(root, root);
    auto c = test_ctx(root, root);
    auto aa = test_ctx(root, a);
    auto ab = test_ctx(root, a);
    auto ac = test_ctx(root, a);

    upp::unused(aa, ab, ac);

    auto* pivot = root.next();
    auto* orig_pivot = pivot;
    auto* curr = orig_pivot;
    ul::organize_next_child(&root, pivot, curr);
    // Grandchildren should be placed after the root in intialization so
    // organizing should not change the node where the pivot points to.
    EXPECT_EQ(orig_pivot, pivot);
    EXPECT_THAT(root.next(), testing::AnyOf(&a, &b, &c));
}

TEST(OrganizeChildren, Root) {
    auto root = test_ctx();
    auto a = test_ctx(root, root);
    auto b = test_ctx(root, root);
    auto c = test_ctx(root, root);
    auto aa = test_ctx(root, a);
    auto ab = test_ctx(root, a);
    auto ac = test_ctx(root, a);

    upp::unused(aa, ab, ac);

    auto* orig_pivot = root.next();
    auto* pivot = ul::organize_children(&root, orig_pivot);
    // Grandchildren should be placed after the root in intialization so
    // organizing should not change the node where the pivot points to.
    ASSERT_EQ(orig_pivot, pivot);

    auto children = all_children_of(&root, &root);
    ASSERT_THAT(children, UnorderedElementsAre(&a, &b, &c));
}

TEST(Organize, One) {
    auto root = test_ctx();
    ul::organize(&root);
}

TEST(Organize, Complex) {
    auto root = test_ctx();
    auto a = test_ctx(root, root);
    auto b = test_ctx(root, root);
    auto aa = test_ctx(root, a);
    auto ab = test_ctx(root, a);
    auto c = test_ctx(root, root);
    auto ca = test_ctx(root, c);
    auto cb = test_ctx(root, c);

    ul::organize(&root);

    auto children = children_of(&root);
    ASSERT_THAT(children, UnorderedElementsAre(&a, &b, &c));
    children = children_of(&a);
    EXPECT_THAT(children, UnorderedElementsAre(&aa, &ab));
    children = children_of(&b);
    EXPECT_TRUE(children.empty());
    children = children_of(&c);
    EXPECT_THAT(children, UnorderedElementsAre(&ca, &cb));
    children = children_of(&aa);
    ASSERT_TRUE(children.empty());
    children = children_of(&ab);
    ASSERT_TRUE(children.empty());
    children = children_of(&ca);
    ASSERT_TRUE(children.empty());
    children = children_of(&cb);
    ASSERT_TRUE(children.empty());
}
