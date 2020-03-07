#pragma once

#include <mutex>

namespace upp {
namespace sync {
template <typename T>
class MutexWrap;

template <typename T>
class MutexTryGuard {
 public:
    ~MutexTryGuard() {
        if (_pval) _mut.unlock();
    }
    operator bool() const { return _pval != nullptr; }
    T& operator*() {
        if (_pval) return *_pval;
        throw std::runtime_error("Lock not acquired");
    }

    T* operator->() {
        if (_pval) return _pval;
        throw std::runtime_error("Lock not acquired");
    }

 private:
    MutexTryGuard(std::mutex& mutex, T& val) : _mut{mutex}, _pval{&val} {
        if (!_mut.try_lock()) _pval = nullptr;
    }

    std::mutex& _mut;
    T* _pval;
    friend MutexWrap<T>;
};

}  // namespace sync
}  // namespace upp
