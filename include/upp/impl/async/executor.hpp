#pragma once

#include "upp/impl/async/schedulable.hpp"

namespace upp {
namespace async {
class Executor {
public:
		virtual void schedule(Schedulable* sched) = 0;
};
}  // namespace async
}  // namespace upp