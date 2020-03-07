#pragma once

#include <mutex>

#include "upp/sync/mutexguard.hpp"
#include "upp/sync/mutextryguard.hpp"

namespace upp {
namespace sync {

template <typename T>
class MutexWrap {
 public:
    MutexWrap(T val) : _mut{}, _val{val} {}

    MutexGuard<T> lock() { return MutexGuard<T>(_mut, _val); }
    MutexTryGuard<T> try_lock() { return MutexTryGuard<T>(_mut, _val); }

 private:
    std::mutex _mut;
    T _val;
};

}  // namespace sync
}  // namespace upp
