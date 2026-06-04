#pragma once

#include <exec/task.hpp>
#include <stdexec/execution.hpp>

namespace upp::coro {

template <class T>
using task = stdexec::task<T>;

}  // namespace upp::coro
