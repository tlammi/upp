#include <gtest/gtest.h>
#include <atomic>
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

TEST(MutexWrapTest, MutexGuard) {
    upp::sync::MutexWrap<int> a{0};
    std::atomic_bool flag{false};
    upp::sync::Thread t{[&]() {
        {
            while (!flag) {}
            auto lock1{a.lock()};
            EXPECT_EQ(*lock1, 1);
            ++*lock1;
        }
    }};
    {
        auto lock0{a.lock()};
        EXPECT_EQ(*lock0, 0);
        ++(*lock0);
    }
    flag = true;
    t.join();
    EXPECT_EQ(*a.lock(), 2);
}

TEST(MutexWrapTest, MutexTryGuard) {
    struct Tmp {
        int val;
    };
    upp::sync::MutexWrap<Tmp> a{Tmp{1}};
    {
        auto lock1{a.try_lock()};
        ASSERT_TRUE(lock1);
        upp::sync::Thread t{[&]() {
            auto lock2{a.try_lock()};
            ASSERT_FALSE(lock2);
            bool caught{false};
            try {
                *lock2;
            } catch (...) { caught = true; }
            ASSERT_TRUE(caught);
            caught = false;
            try {
                lock2->val;
            } catch (...) { caught = true; }
            ASSERT_TRUE(caught);
        }};

        t.join();
    }
}
