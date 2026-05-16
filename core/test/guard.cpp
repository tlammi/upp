#include <gtest/gtest.h>

#include <upp/guard.hpp>

struct mock_mutex {
    bool locked{};

    void lock() noexcept { locked = true; }
    void unlock() noexcept { locked = false; }
};

TEST(Ctor, Default) {
    auto v = upp::guard<int>();
    auto lk = v.lock();
    ASSERT_EQ(*lk, 0);
}

TEST(Ctor, Val) {
    auto v = upp::guard<int>(1);
    auto lk = v.lock();
    ASSERT_EQ(*lk, 1);
}

TEST(Ctor, Inplace) {
    auto v = upp::guard<int>(std::in_place, 1);
    auto lk = v.lock();
    ASSERT_EQ(*lk, 1);
}

template <class T>
using test_guard = upp::basic_guard<T, mock_mutex>;

TEST(Locking, Basic) {
    auto v = test_guard<int>();
    ASSERT_FALSE(v.mutex().locked);
    {
        auto lk = v.lock();
        ASSERT_TRUE(v.mutex().locked);
    }
    ASSERT_FALSE(v.mutex().locked);
}
