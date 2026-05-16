#pragma once

#include <cassert>
#include <cstddef>
#include <memory>
#include <upp/detail/allocator_holder.hpp>
#include <upp/flexible.hpp>
#include <utility>

namespace upp {
namespace ringbuffer_detail {
template <class Alloc>
using alloc_holder = detail::allocator_holder<
    typename std::allocator_traits<Alloc>::template rebind_alloc<std::byte>>;
}

template <class T, class Alloc = std::allocator<T>>
class ring_buffer : ringbuffer_detail::alloc_holder<Alloc> {
    struct header {
        size_t capacity{};
        size_t offset{};
        size_t count{};
    };
    using flex = flexible<T, header>;
    flex* m_dat{};

    using ringbuffer_detail::alloc_holder<Alloc>::get_allocator;

 public:
    constexpr ring_buffer() noexcept = default;

    constexpr explicit ring_buffer(size_t capacity)
        : m_dat(flex::create(capacity, get_allocator())) {
        m_dat->header.capacity = capacity;
    }

    ring_buffer(const ring_buffer&) = delete;
    ring_buffer& operator=(const ring_buffer&) = delete;

    ring_buffer(ring_buffer&& other) noexcept
        : m_dat(std::exchange(other.m_dat, nullptr)) {}

    ring_buffer& operator=(ring_buffer&& other) noexcept {
        std::destroy_at(this);
        std::construct_at(this, std::move(other));
        return *this;
    }
    ~ring_buffer() {
        if (!m_dat) return;
        flex::destroy(m_dat, get_allocator(), m_dat->header.capacity);
    }

    constexpr size_t capacity() const noexcept {
        if (!m_dat) [[unlikely]]
            return 0;
        return m_dat->header.capacity;
    }

    constexpr size_t size() const noexcept {
        if (!m_dat) [[unlikely]]
            return 0;
        return m_dat->header.count;
    }

    constexpr bool empty() const noexcept {
        if (!m_dat) [[unlikely]]
            return true;
        return m_dat->header.count == 0;
    }

    template <class S>
    decltype(auto) front(this S& s) noexcept {
        assert(!s.empty());
        return s.m_dat->data()[s.m_dat->header.offset];
    }

    template <class... Ts>
    void push(Ts&&... ts) {
        assert(m_dat);
        auto count = m_dat->header.count;
        auto cap = m_dat->header.capacity;
        auto idx = (m_dat->header.offset + count) % cap;
        if (count == cap) {
            std::destroy_at(&m_dat->data()[idx]);
            ++m_dat->header.offset;
        } else {
            ++m_dat->header.count;
        }
        std::construct_at(&m_dat->data()[idx], std::forward<Ts>(ts)...);
    }

    void pop() noexcept {
        assert(!empty());
        std::destroy_at(&m_dat->data()[m_dat->header.offset]);
        ++m_dat->header.offset;
        --m_dat->header.count;
    }
};

}  // namespace upp
