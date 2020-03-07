#pragma once

#include <mutex>

namespace upp {
namespace sync {

template <typename T>
class MutexWrap;

template <typename T>
class MutexGuard {
 public:
    T& operator*() { return _val; }
    T* operator->() { return &_val; }
    ~MutexGuard() { _mut.unlock(); }

 private:
    MutexGuard(std::mutex& mutex, T& val) : _mut{mutex}, _val{val} {
        mutex.lock();
    }

    std::mutex& _mut;
    T& _val;
    friend MutexWrap<T>;
};
}  // namespace sync
}  // namespace upp