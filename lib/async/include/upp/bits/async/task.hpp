#pragma once

#include <cassert>
#include <coroutine>
#include <exception>
#include <upp/bits/async/ctx.hpp>
#include <upp/bits/async/scheduler.hpp>
#include <upp/bits/async/unique_handle.hpp>
#include <upp/util.hpp>

namespace upp::async {

template <class T>
class TaskPromise;

template <class T>
class TaskAwaiter {
 public:
    using handle_type = std::coroutine_handle<TaskPromise<T>>;

 private:
    handle_type m_dst;

 public:
    constexpr explicit TaskAwaiter(handle_type handle) noexcept
        : m_dst(handle) {}

    constexpr bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> awaiter) const noexcept {
        auto prev = detail::scheduler().replace_curr(m_dst);
        std::println("{} == {}", prev.address(), awaiter.address());
        assert(prev.address() == awaiter.address());
        m_dst.promise().set_continuation(awaiter);
        std::println("task await_suspend");
    }

    T await_resume() {
        std::println("task await_resume");
        if constexpr (!std::same_as<T, void>) {
            if constexpr (std::move_constructible<T>) {
                return std::move(m_dst.promise().value());
            } else {
                return m_dst.promise().value();
            }
        }
    }
};

template <class T = void>
class Task final : private UniqueHandle<TaskPromise<T>> {
    using Parent = UniqueHandle<TaskPromise<T>>;

    UPP_ASSERT_CONDITION_NO_UNIQUE_ADDRESS AssertCondition m_used{
        "Task<> not awaited or detached"};

 public:
    using promise_type = TaskPromise<T>;
    using handle_type = std::coroutine_handle<promise_type>;
    explicit Task(handle_type h) : Parent(h) {}

    Parent extract() {
        m_used = true;
        return Parent(std::move(*this));
    }

    void operator+() && {
        m_used = true;
        detail::scheduler().detach(std::move(*this));
    }

    TaskAwaiter<T> operator co_await() {
        m_used = true;
        std::println("task co_await");
        return TaskAwaiter<T>{Parent::get()};
    }
};

template <class Tsk>
class TaskPromiseBase {
    std::coroutine_handle<> m_continuation{};

 public:
    constexpr std::suspend_always initial_suspend() const noexcept {
        return {};
    }

    std::suspend_always final_suspend() const noexcept {
        std::println("final_suspend");
        // null if detached
        if (m_continuation) detail::scheduler().replace_curr(m_continuation);
        std::println("final_suspend 2");
        return {};
    }

    constexpr void set_continuation(
        std::coroutine_handle<> continuation) noexcept {
        m_continuation = continuation;
    }

    void unhandled_exception() { std::terminate(); }

 protected:
    ~TaskPromiseBase() = default;
};

template <class T>
class TaskPromise : public TaskPromiseBase<TaskPromise<T>> {
    // TODO: could this be an union?
    std::optional<T> m_val{};

 public:
    void return_value(T value) { m_val.emplace(std::move(value)); }

    template <class Self>
    decltype(auto) value(this Self&& self) {
        return *std::forward<Self>(self).m_val;
    }

    Task<T> get_return_object() {
        return Task<T>{std::coroutine_handle<TaskPromise>::from_promise(*this)};
    }
};

template <>
class TaskPromise<void> : public TaskPromiseBase<TaskPromise<void>> {
 public:
    void return_void() {}
    Task<> get_return_object() {
        return Task<>{std::coroutine_handle<TaskPromise>::from_promise(*this)};
    }
};
}  // namespace upp::async
