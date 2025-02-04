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
    bool active{true};

    SchedCtx(UniqueHandle<> root, Scheduler* sched)
        : root(std::move(root)), sched(sched) {
        stack.push(this->root.get());
    }

    void activate() final { active = true; }
    void deactivate() final { active = false; }

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
    std::list<detail::SchedCtx>::iterator m_iter{m_ctxs.begin()};

 public:
    Scheduler() = default;
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;
    // TODO: enable these. Need to update sched field in m_ctxs
    // See also upp::async::scheduler() which gets the address of this
    // Maybe reconsider that? Could task detaching be done without publishing
    // this into the coroutines?
    Scheduler(Scheduler&&) = delete;
    Scheduler& operator=(Scheduler&&) = delete;

    ~Scheduler() = default;

    void dispatch(Task<void> t);

    bool done() const noexcept;
    void step();
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
