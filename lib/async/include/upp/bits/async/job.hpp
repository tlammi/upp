#pragma once

#include <upp/bits/async/task.hpp>
#include <utility>

namespace upp::async {

class Job {
    Task<void> m_task;

 public:
    explicit Job(Task<void> t) noexcept : m_task(std::move(t)) {}

    bool done() const noexcept;
    void step();
    void run() {
        while (!done()) step();
    }
};
}  // namespace upp::async
