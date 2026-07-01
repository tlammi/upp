#pragma once

#include <atomic>
#include <concepts>

namespace upp::lockfree {

template <class T>
concept intrusive_node = !std::movable<T>;

template <class T>
struct intrusive_node_traits {
    static constexpr std::atomic<T*>& next(T* curr) noexcept {
        return curr->next();
    }
};

}  // namespace upp::lockfree
