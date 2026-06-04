#pragma once

#include <upp/coro/task.hpp>

namespace upp::coro {

template <class Scope, class Sender>
task<void> detach(Scope& scope, Sender&& sender) {
    auto sched = co_await stdexec::get_scheduler();
    scope.spawn(stdexec::starts_on(sched, std::forward<Sender>(sender)));
}

}  // namespace upp::coro
