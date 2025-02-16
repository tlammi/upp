#pragma once

#include <coroutine>
#include <mutex>
#include <upp/bits/async/task.hpp>
#include <upp/types.hpp>

namespace upp::async {

class MutexLocker;

class Mutex {
    friend class MutexLocker;
    bool m_locked{false};
    RingBuf<std::coroutine_handle<>> m_awaiters{};

 public:
    Mutex() = default;
    Mutex(const Mutex&) noexcept = default;
    Mutex& operator=(const Mutex&) noexcept = default;
    Mutex(Mutex&&) noexcept = default;
    Mutex& operator=(Mutex&&) noexcept = default;

    ~Mutex() = default;

    [[nodiscard]] MutexLocker lock();

    void unlock() {
        m_locked = false;
        if (m_awaiters.empty()) return;
        detail::scheduler().reactivate(m_awaiters.front());
        m_awaiters.pop_front();
    }

    bool locked() const noexcept { return m_locked; }
};

class MutexLocker {
    Mutex* m_mut{};
    friend class Mutex;
    constexpr MutexLocker(Mutex* mut) noexcept : m_mut(mut) {}

 public:
    constexpr bool await_ready() const noexcept { return !m_mut->m_locked; }

    void await_suspend(std::coroutine_handle<> handle) {
        auto deactivated = detail::scheduler().deactivate_curr();
        assert(deactivated == handle);
        m_mut->m_awaiters.push_back(deactivated);
    }

    void await_resume() { m_mut->m_locked = true; }
};

template <template <class...> class Lock, class Mutex>
[[nodiscard]] Task<Lock<std::remove_cvref_t<Mutex>>> lock(Mutex& mut) {
    co_await mut.lock();
    co_return Lock<std::remove_cvref_t<Mutex>>{mut, std::adopt_lock};
}

template <class Mutex>
[[nodiscard]] Task<std::unique_lock<std::remove_cvref_t<Mutex>>> unique_lock(
    Mutex& mut) {
    return lock<std::unique_lock>(mut);
}

MutexLocker Mutex::lock() { return {this}; }

}  // namespace upp::async
