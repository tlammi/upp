#pragma once

#include <list>
#include <upp/bits/async/semaphore.hpp>
#include <upp/bits/async/task.hpp>

namespace upp::async {

template <class T>
class Channel {
    Semaphore m_sem{};
    std::list<T> m_messages{};

 public:
    Task<T> read() {
        std::println("read 1");
        co_await m_sem.acquire();
        std::println("read 2");
        auto v = std::move(m_messages.front());
        std::println("read 3");
        m_messages.pop_front();
        std::println("read 4");
        co_return v;
    }

    template <class... Ts>
    void write(Ts&&... ts) {
        std::println("write 1");
        m_messages.emplace_back(std::forward<Ts>(ts)...);
        std::println("write 2");
        m_sem.release();
        std::println("write 3");
    }

    bool empty() const noexcept { return !m_messages.size(); }
    size_t size() const noexcept { return m_messages.count(); }
};

template <>
class Channel<void> {
    Semaphore m_sem{};

 public:
    Task<> read() { co_await m_sem.acquire(); }

    void write() { m_sem.release(); }

    bool empty() const noexcept { return !m_sem.count(); }
    size_t size() const noexcept { return m_sem.count(); }
};
}  // namespace upp::async
