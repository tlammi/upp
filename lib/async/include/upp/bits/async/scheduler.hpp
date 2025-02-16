#pragma once

#include <cassert>
#include <upp/bits/async/unique_handle.hpp>
#include <vector>

namespace upp::async {

class Scheduler {
 public:
    /**
     * \brief Detach a new task
     * */
    virtual void detach(UniqueHandle<> h) = 0;

    /**
     * \brief Replace the currently active coroutine
     *
     * Replace the currently active coroutine with a new one and return the
     * replaced one. This is typically called from awaiter which replaces the
     * current coroutine with its target (and restores the original coroutine
     * once awaiting has finished).
     * */
    virtual std::coroutine_handle<> replace_curr(
        std::coroutine_handle<> handle) = 0;

    /**
     * \brief Deactivate current coroutine.
     *
     * This extracts the current coroutine so it is not executed by the
     * framework. Called e.g. when a mutex blocks a coroutine.
     * */
    virtual std::coroutine_handle<> deactivate_curr() = 0;

    /**
     * \brief Reactivate a previously deactivated coroutine
     * */
    virtual void reactivate(std::coroutine_handle<> handle) = 0;

 protected:
    constexpr ~Scheduler() = default;
};

namespace detail {
inline thread_local Scheduler* g_scheduler{};  // NOLINT
void set_scheduler(Scheduler& sched) { g_scheduler = &sched; }
void clear_scheduler() { g_scheduler = nullptr; }
Scheduler& scheduler() {
    assert(g_scheduler);
    return *g_scheduler;
}
}  // namespace detail

}  // namespace upp::async

#if 0
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

// TODO: Should this be a global instance?
// The current approach forces the corotuines to be lazy.
// Using a global instance would allow for eager coroutines.
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
#endif
