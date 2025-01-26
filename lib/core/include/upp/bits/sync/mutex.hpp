#pragma once

#include <mutex>
#include <optional>
#include <shared_mutex>
#include <upp/concepts.hpp>

namespace upp {

template <class T, class Lock>
class BasicMutexLock {
 public:
    BasicMutexLock(T& val, Lock lk) noexcept
        : m_lock(std::move(lk)), m_val(&val) {}

    T& operator*() const noexcept { return *m_val; }
    T* operator->() const noexcept { return m_val; }

 private:
    Lock m_lock;
    T* m_val;
};

template <class T, class Mut>
class BasicMutex {
 public:
    BasicMutex() noexcept = default;

    explicit BasicMutex(const T& v) noexcept(
        std::is_nothrow_copy_constructible_v<T>)
        requires(std::copy_constructible<T>)
        : m_val(v) {}

    explicit BasicMutex(T&& v) noexcept(std::is_nothrow_move_constructible_v<T>)
        requires(std::move_constructible<T>)
        : m_val(std::move(v)) {}

    template <class... Ts>
    explicit BasicMutex(std::in_place_t, Ts&&... ts)
        requires(std::is_constructible_v<T, Ts...>)
        : m_val(std::forward<Ts>(ts)...) {}

    auto lock()
        requires(concepts::basic_lockable<Mut>)
    {
        return BasicMutexLock{m_val, std::unique_lock(m_mut)};
    }

    auto lock_shared() const
        requires(concepts::shared_lockable<Mut>)
    {
        return BasicMutexLock(m_val, std::shared_lock(m_mut));
    }

    std::optional<BasicMutexLock<T, std::unique_lock<Mut>>> try_lock()
        requires(concepts::lockable<Mut>)
    {
        auto lk = std::unique_lock(m_mut, std::try_to_lock);
        if (!lk.owns_lock()) return std::nullopt;
        return BasicMutexLock(m_val, std::move(lk));
    }

 private:
    mutable Mut m_mut{};
    T m_val{};
};

template <class T>
using Mutex = BasicMutex<T, std::mutex>;

}  // namespace upp
