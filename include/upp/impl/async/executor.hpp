#pragma once

#include <algorithm>
#include <condition_variable>
#include <deque>
#include <iostream>
#include <mutex>
#include <set>
#include <thread>

#include "upp/impl/async/schedulable.hpp"

namespace upp {
namespace async {
class Executor {
public:
		virtual void schedule(Schedulable& sched, int priority) = 0;
		virtual void cancel(Schedulable& sched) = 0;
};

}  // namespace async
}  // namespace upp