#pragma once

#include <cassert>
#include <coroutine>
#include <exception>
#include <upp/bits/async/ctx.hpp>
#include <upp/bits/async/scheduler.hpp>
#include <upp/bits/async/unique_handle.hpp>

namespace upp::async {

template <class T>
class TaskPromise;

template <class T>
class TaskAwaiter {
 public:
    using handle_type = std::coroutine_handle<TaskPromise<T>>;

 private:
    handle_type m_handle;

 public:
    constexpr explicit TaskAwaiter(handle_type handle) noexcept
        : m_handle(handle) {}

    constexpr bool await_ready() const noexcept { return false; }

    template <class Promise>
    void await_suspend(std::coroutine_handle<Promise> parent) {
        auto& ctx = parent.promise().context();
        m_handle.promise().set_context(ctx);
        ctx.push_stack(m_handle);
    }

    T await_resume() {
        if constexpr (!std::same_as<T, void>) return m_handle.promise().value();
    }
};

template <class T = void>
class Task final : private UniqueHandle<TaskPromise<T>> {
    using Parent = UniqueHandle<TaskPromise<T>>;

 public:
    using promise_type = TaskPromise<T>;
    using handle_type = std::coroutine_handle<promise_type>;
    explicit Task(handle_type h) : Parent(h) {}

    Parent extract() { return Parent(std::move(*this)); }

    [[nodiscard]] Task<void> operator+() && {
        auto& s = co_await scheduler();
        s.dispatch(std::move(*this));
    }

    TaskAwaiter<T> operator co_await() { return TaskAwaiter<T>{Parent::get()}; }
};

class TaskPromiseBase {
    Ctx* m_ctx{};

 public:
    // TODO: should this be suspend_never
    constexpr std::suspend_always initial_suspend() const noexcept {
        return {};
    }

    constexpr std::suspend_always final_suspend() const noexcept { return {}; }

    void unhandled_exception() { std::terminate(); }

    void set_context(Ctx& ctx) noexcept { m_ctx = &ctx; }
    Ctx& context() const noexcept {
        assert(m_ctx);
        return *m_ctx;
    }

 protected:
    ~TaskPromiseBase() = default;
};

template <class T>
class TaskPromise : public TaskPromiseBase {
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
class TaskPromise<void> : public TaskPromiseBase {
 public:
    void return_void() {}
    Task<> get_return_object() {
        return Task<>{std::coroutine_handle<TaskPromise>::from_promise(*this)};
    }
};
}  // namespace upp::async
