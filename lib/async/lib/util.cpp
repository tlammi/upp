#include <upp/bits/async/scheduler.hpp>
#include <upp/bits/async/util.hpp>

namespace upp::async {

void run(Task<void> t) {
    auto sched = Scheduler();
    sched.dispatch(std::move(t));
    sched.run();
}
}  // namespace upp::async
