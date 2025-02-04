#pragma once

#include <cstddef>
#include <list>
#include <upp/bits/async/ctx.hpp>

namespace upp::async {

class SemAcquirer {
    size_t* m_counter;
    std::list<Ctx*>* m_awaiters;

 public:
    constexpr SemAcquirer(size_t* counter, std::list<Ctx*>* awaiters) noexcept
        : m_counter(counter), m_awaiters(awaiters) {}

    SemAcquirer(const SemAcquirer&) = delete;
    SemAcquirer& operator=(const SemAcquirer&) = delete;

    SemAcquirer(SemAcquirer&&) = delete;
    SemAcquirer& operator=(SemAcquirer&&) = delete;

    constexpr ~SemAcquirer() = default;

    constexpr bool await_ready() const noexcept { return *m_counter; }

    template <class Promise>
    void await_suspend(std::coroutine_handle<Promise> handle) {
        auto* ctx = &handle.promise().context();
        ctx->deactivate();
        m_awaiters->push_back(ctx);
    }

    constexpr void await_resume() const noexcept { --*m_counter; }
};

class Semaphore {
    size_t m_counter{0};
    // TODO: is there a better type?
    std::list<Ctx*> m_awaiters{};

 public:
    explicit Semaphore(size_t count = 0) noexcept : m_counter(count) {}

    [[nodiscard]] SemAcquirer acquire() { return {&m_counter, &m_awaiters}; }

    [[nodiscard]] bool try_acquire() noexcept {
        if (m_counter) {
            --m_counter;
            return true;
        }
        return false;
    }

    void release(size_t count = 1) {
        while (!m_awaiters.empty() && count) {
            m_awaiters.front()->activate();
            m_awaiters.pop_front();
            --count;
        }
        m_counter += count;
    }

    size_t count() const noexcept { return m_counter; }
};

}  // namespace upp::async
