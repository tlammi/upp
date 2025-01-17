#pragma once

#include <mutex>
#include <type_traits>
#include <upp/immovable.hpp>
#include <upp/uncopyable.hpp>
#include <utility>

namespace upp {

namespace basic_guard_detail {

template <class T>
concept mutexlike = requires(T t) {
    { t.lock() };
    { t.unlock() };
};
}  // namespace basic_guard_detail
/**
 * \brief Type with an integrated mutex
 *
 * This type provides a thread safe access to a value without a need to manually
 * synchronize threads.
 * */
template <class T, basic_guard_detail::mutexlike Mutex>
class basic_guard : public immovable, public uncopyable {
    Mutex m_mut{};
    T m_v{};

    class raii_lock {
        friend basic_guard;
        basic_guard* m_parent;
        explicit raii_lock(basic_guard* parent) noexcept : m_parent(parent) {
            m_parent->m_mut.lock();
        }

     public:
        raii_lock(const raii_lock&) = delete;
        raii_lock& operator=(const raii_lock&) = delete;

        raii_lock(raii_lock&& other) noexcept
            : m_parent(std::exchange(other.m_parent, nullptr)) {}

        raii_lock& operator=(raii_lock&& other) noexcept {
            std::destroy_at(this);
            std::construct_at(this, std::move(other));
            return *this;
        }

        ~raii_lock() {
            if (m_parent) m_parent->m_mut.unlock();
        }

        template <class S>
        decltype(auto) operator*(this S& self) {
            assert(self.m_parent);
            return self.m_parent->m_v;
        }
        template <class S>
        T* operator->(this S& self) {
            assert(self.m_parent);
            return &self.m_parent->m_v;
        }
    };

 public:
    basic_guard() noexcept(std::is_nothrow_constructible_v<T>) = default;

    explicit basic_guard(const T& t) noexcept(
        std::is_nothrow_copy_constructible_v<T>)
        requires(std::is_copy_constructible_v<T>)
        : m_v(t) {}

    explicit basic_guard(T&& t) noexcept(
        std::is_nothrow_move_constructible_v<T>)
        requires(std::is_move_constructible_v<T>)
        : m_v(std::move(t)) {}

    template <class... Ts>
    explicit basic_guard(std::in_place_t, Ts&&... ts) noexcept(
        std::is_nothrow_constructible_v<T, Ts...>)
        : m_v(std::forward<Ts>(ts)...) {}

    auto lock() { return raii_lock(this); }

    template <class S>
    auto& mutex(this S& self) noexcept {
        return self.m_mut;
    }
};

template <class T>
using guard = basic_guard<T, std::mutex>;

}  // namespace upp
