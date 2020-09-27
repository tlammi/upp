/* SPDX-License-Identifier: MIT */
/* Copyright @ 2020 Toni Lammi */
#pragma once

#include <condition_variable>
#include <cstdlib>
#include <mutex>
#include <optional>

namespace upp {
namespace sync {

template <typename T, size_t S>
class Fifo {
 public:
    Fifo() : _mut{}, _cv{}, _windex{0}, _count{0} {}

    void push(T val) {
        size_t windex;
        {
            std::unique_lock<std::mutex> lock{_mut};
            if (_count < S) ++_count;
            windex = _windex;
            _windex = Fifo<T, S>::_wrap_inc(_windex);
            _buf[windex] = val;
        }
        _cv.notify_one();
    }

    bool try_push(T val) {
        size_t windex;
        {
            std::unique_lock<std::mutex> lock{_mut};
            if (_count == S) return false;
            ++_count;
            windex = _windex;
            _windex = Fifo<T, S>::_wrap_inc(_windex);
            _buf[windex] = val;
        }
        _cv.notify_one();
        return true;
    }

    T pop() {
        size_t rindex;
        {
            std::unique_lock<std::mutex> lock{_mut};
            while (!_count) _cv.wait(lock);
            rindex = Fifo<T, S>::_calc_rindex(_windex, _count);
            --_count;
        }
        return _buf[rindex];
    }

    std::optional<T> try_pop() {
        size_t rindex;
        {
            std::unique_lock<std::mutex> lock{_mut};
            if (!_count) return std::nullopt;
            rindex = Fifo<T, S>::_calc_rindex(_windex, _count);
            --_count;
        }
        return std::make_optional(_buf[rindex]);
    }

    size_t count() {
        std::unique_lock<std::mutex> lock{_mut};
        return _count;
    }

    constexpr size_t capacity() { return S; }

 private:
    static size_t inline _wrap_inc(size_t in) { return (in + 1) % S; }
    static size_t inline _calc_rindex(size_t windex, size_t count) {
        if (windex >= count) return windex - count;
        return S - count + windex;
    }
    std::mutex _mut;
    std::condition_variable _cv;
    size_t _windex;
    size_t _count;
    T _buf[S];
};

}  // namespace sync
}  // namespace upp