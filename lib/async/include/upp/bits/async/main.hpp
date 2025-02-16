#pragma once

#include <cassert>
#include <coroutine>
#include <exception>
#include <print>
#include <upp/bits/async/executor.hpp>
#include <upp/bits/async/scheduler.hpp>
#include <upp/bits/async/unique_handle.hpp>
#include <upp/types.hpp>
#include <vector>

namespace upp::async {

struct MainOpts {
    Executor* executor;
};

class MainPromise;

class Main final : Scheduler {
    friend class MainPromise;

    std::vector<UniqueHandle<>> m_root{};
    RingBuf<std::coroutine_handle<>> m_tasks{};

    int m_result{};

    constexpr explicit Main(std::coroutine_handle<MainPromise> prom) noexcept;

    void detach(UniqueHandle<> h) override {
        std::println("detach: {}", h.address());
        m_tasks.push_back(h.get());
        auto iter =
            std::find_if(m_root.begin(), m_root.end(),
                         [](const auto& handle) { return handle.done(); });
        if (iter != m_root.end()) {
            *iter = std::move(h);
            return;
        }
        m_root.push_back(std::move(h));
    }

    std::coroutine_handle<> replace_curr(
        std::coroutine_handle<> handle) override {
        std::println("replace {} -> {}", m_tasks.front().address(),
                     handle.address());
        auto prev = std::exchange(m_tasks.front(), handle);
        return prev;
    }

    std::coroutine_handle<> deactivate_curr() override {
        auto h = m_tasks.front();
        m_tasks.pop_front();
        return h;
    }

    void reactivate(std::coroutine_handle<> handle) override {
        m_tasks.push_back(handle);
    }

 public:
    using promise_type = MainPromise;

    constexpr int run() {
        detail::set_scheduler(*this);
        while (!done()) step();
        detail::clear_scheduler();
        return result();
    }

    constexpr bool done() const noexcept;
    constexpr void step();
    constexpr int result() { return m_result; }
};

class MainPromise {
    friend class Main;
    int* m_res{};

 public:
    constexpr std::suspend_always initial_suspend() const noexcept {
        return {};
    }
    constexpr std::suspend_always final_suspend() const noexcept { return {}; }
    constexpr void return_value(int v) { *m_res = v; }

    void unhandled_exception() { std::terminate(); }

    constexpr auto get_return_object() {
        return Main{std::coroutine_handle<MainPromise>::from_promise(*this)};
    }
};

constexpr Main::Main(std::coroutine_handle<MainPromise> prom) noexcept {
    m_root.emplace_back(prom);
    m_tasks.push_back(m_root.back().get());
    prom.promise().m_res = &m_result;
}

constexpr bool Main::done() const noexcept {
    std::println("done()?");
    return m_tasks.empty();
}

constexpr void Main::step() {
    std::println("step: {}", m_tasks.front().address());
    m_tasks.front().resume();
    if (m_tasks.front().done()) {
        m_tasks.pop_front();
        return;
    }
    m_tasks.rotate();
}

}  // namespace upp::async
