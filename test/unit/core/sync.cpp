#include <gtest/gtest.h>

#include <upp/sync.hpp>

TEST(Mutex, CtorDefault) {
    auto m = upp::Mutex<int>();
    auto lk = m.lock();
    ASSERT_EQ(*lk, int{});
}

TEST(Mutex, CtorVal) {
    auto m = upp::Mutex<int>{1};
    auto lk = m.lock();
    ASSERT_EQ(*lk, 1);
}

struct MockMut {
    bool locked = false;
    void lock() noexcept { locked = true; }
    void unlock() noexcept { locked = false; }

    bool try_lock() {
        auto was_locked = std::exchange(locked, true);
        return !was_locked;
    }
};

TEST(Mutex, LockAcquireFree) {
    MockMut mut{};
    int dummy{};
    {
        auto lk = upp::BasicMutexLock(dummy, std::unique_lock(mut));
        ASSERT_TRUE(mut.locked);
    }
    ASSERT_FALSE(mut.locked);
}

TEST(Mutex, TryLockSuccess) {
    auto m = upp::BasicMutex<int, MockMut>(1);
    auto lk = m.try_lock();
    ASSERT_TRUE(lk);
    ASSERT_EQ(**lk, 1);
}

TEST(Mutex, TryLockFail) {
    auto m = upp::BasicMutex<int, MockMut>(1);
    auto lk_a = m.lock();
    auto lk_b = m.try_lock();
    ASSERT_FALSE(lk_b);
}

struct MockSharedMut {
    bool locked = false;
    size_t shared_locked = 0;

    void lock() {
        assert(!locked);
        assert(!shared_locked);
        locked = true;
    }

    bool try_lock() {
        if (locked || shared_locked) return false;
        return !std::exchange(locked, true);
    }

    void unlock() {
        assert(locked);
        assert(!shared_locked);
        locked = false;
    }

    void lock_shared() {
        assert(!locked);
        ++shared_locked;
    }

    bool try_lock_shared() {
        if (locked) return false;
        ++shared_locked;
        return true;
    }

    void unlock_shared() {
        assert(shared_locked);
        assert(!locked);
        --shared_locked;
    }
};

TEST(Mutex, LockShared) {
    auto m = upp::BasicMutex<int, MockSharedMut>(1);
    {
        auto lk_a = m.lock_shared();
        auto lk_b = m.lock_shared();
        auto lk_c = m.try_lock();
        ASSERT_FALSE(lk_c);
    }
    auto lk = m.try_lock();
    ASSERT_TRUE(lk);
}
