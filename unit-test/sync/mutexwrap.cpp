#include <gtest/gtest.h>
#include "upp/sync.hpp"

TEST(MutexWrapTest, IndirectionOperator) {
    upp::sync::MutexWrap<int> a{100};
    ASSERT_EQ(*a.lock(), 100);
}

TEST(MutexWrapTest, MemberOfPointerOperator) {
    struct Tmp {
        int a;
        double b;
    };

    upp::sync::MutexWrap<Tmp> a{Tmp{100, 2.0}};
    ASSERT_EQ(a.lock()->a, 100);
    ASSERT_EQ(a.try_lock()->b, 2.0);
}