#pragma once
#include <cassert>
#include <list>
#include <print>
#include <stack>
#include <upp/bits/async/ctx.hpp>
#include <upp/bits/async/unique_handle.hpp>

namespace upp::async {
class Scheduler;
namespace detail {

struct SchedCtx final : public Ctx {
    using Stack = std::stack<std::coroutine_handle<>>;
    UniqueHandle<> root{};
    Stack stack{};
    Scheduler* sched{};

    SchedCtx(UniqueHandle<> root, Scheduler* sched)
        : root(std::move(root)), sched(sched) {
        stack.push(this->root.get());
    }

    void activate() final {}
    void deactivate() final {}

    void push_stack(std::coroutine_handle<> handle) override {
        stack.push(handle);
    }

    Scheduler& scheduler() const noexcept override { return *sched; }
};
}  // namespace detail

template <class T>
class Task;

class Scheduler {
    std::list<detail::SchedCtx> m_ctxs{};

 public:
    Scheduler() = default;
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;
    // TODO: enable these. Need to update sched field in m_ctxs
    Scheduler(Scheduler&&) = delete;
    Scheduler& operator=(Scheduler&&) = delete;

    ~Scheduler() = default;

    void dispatch(Task<void> t);

    void run();
};

class SchedulerAwaiter {
    Scheduler* m_sched{};

 public:
    constexpr bool await_ready() const noexcept { return false; }

    template <class Promise>
    void await_suspend(std::coroutine_handle<Promise> parent) {
        m_sched = &parent.promise().context().scheduler();
    }

    Scheduler& await_resume() {
        assert(m_sched);
        return *m_sched;
    }
};

constexpr SchedulerAwaiter scheduler() { return {}; }

}  // namespace upp::async
