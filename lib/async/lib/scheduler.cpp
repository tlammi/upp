#include <print>
#include <upp/bits/async/scheduler.hpp>
#include <upp/bits/async/task.hpp>

namespace upp::async {

void Scheduler::dispatch(Task<> t) {
    std::println("dispatch");
    auto uniq = t.extract();
    auto h = uniq.get();
    m_ctxs.push_back(detail::SchedCtx{std::move(uniq), this});
    auto& last = m_ctxs.back();
    h.promise().set_context(last);
}

bool Scheduler::done() const noexcept { return m_ctxs.empty(); }

void Scheduler::step() {
    if (m_iter == m_ctxs.end()) m_iter = m_ctxs.begin();
    auto& stack = m_iter->stack;
    stack.top().resume();
    if (stack.top().done()) {
        stack.pop();
        if (stack.empty()) {
            auto old = m_iter++;
            m_ctxs.erase(old);
            return;
        }
    }
    ++m_iter;
}

void Scheduler::run() {
    while (!done()) step();
    /*
      auto iter = m_ctxs.begin();
      while (!done()) {
          if (iter == m_ctxs.end()) iter = m_ctxs.begin();
          auto& stack = iter->stack;
          stack.top().resume();
          if (stack.top().done()) {
              stack.pop();
              if (stack.empty()) {
                  auto old = iter++;
                  m_ctxs.erase(old);
                  continue;
              }
          }
          ++iter;
      }
      */
}
}  // namespace upp::async
