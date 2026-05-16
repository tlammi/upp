#pragma once

#include <limits>
#include <memory>
#include <upp/cleanup.hpp>

namespace upp::experimental {
namespace queue_detail {

constexpr auto chunk_bytes = 1024u;

template <class T>
constexpr auto chunk_elements = chunk_bytes / sizeof(T) + 1;

template <class T>
consteval auto chunk_index_type_impl() noexcept {
    const auto count = chunk_elements<T>;
    if constexpr (count <= std::numeric_limits<std::uint8_t>::max())
        return std::uint8_t{};
    if constexpr (count <= std::numeric_limits<std::uint16_t>::max())
        return std::uint16_t{};
}

template <class T>
using chunk_index_type = decltype(chunk_index_type_impl<T>());

template <class Alloc, class = void>
class alloc_holder {
    Alloc m_alloc{};

 protected:
    explicit alloc_holder() = default;
    explicit alloc_holder(Alloc a) : m_alloc(std::move(a)) {}
    constexpr auto get_allocator() const { return Alloc(); }
};

template <class Alloc>
class alloc_holder<
    Alloc, std::enable_if_t<std::allocator_traits<Alloc>::is_always_equal()>> {
 protected:
    explicit alloc_holder() {}
    explicit alloc_holder(Alloc a) { (void)a; }
    constexpr auto get_allocator() const { return Alloc(); }
};

template <class T>
struct chunk {
    chunk* next{};
    union elem {
        char unused{};
        T val;
    };
    std::array<elem, chunk_elements<T>> data{};
};
}  // namespace queue_detail

/**
 * \brief Queue type
 *
 *
 * */
template <class T, class Alloc = std::allocator<T>>
class queue : private queue_detail::alloc_holder<typename std::allocator_traits<
                  Alloc>::template rebind_alloc<queue_detail::chunk<T>>> {
    static constexpr auto chunk_elements = queue_detail::chunk_elements<T>;
    using index_type = queue_detail::chunk_index_type<T>;

    using chunk = queue_detail::chunk<T>;

    auto get_allocator() const {
        return queue_detail::alloc_holder<
            typename std::allocator_traits<Alloc>::template rebind_alloc<
                queue_detail::chunk<T>>>::get_allocator();
    }

 public:
    constexpr queue() noexcept = default;

    constexpr queue(std::initializer_list<T> il) {
        for (auto&& i : il) {
            // TODO: optimize
            push(std::forward<decltype(i)>(i));
        }
    }
    queue(const queue&) = delete;
    queue& operator=(const queue&) = delete;

    // TODO: Implement
    queue(queue&&) = delete;
    queue& operator=(queue&&) = delete;

    constexpr ~queue() {
        if (!m_front) return;
        if (m_front == m_back) {
            for (auto idx = m_idx_front; idx != m_idx_end; ++idx) {
                std::destroy_at(&m_front->data[idx].val);
            }
            return;
        }
        for (auto idx = m_idx_front; idx < chunk_elements; ++idx) {
            std::destroy_at(&m_front->data[idx].val);
        }
        for (auto idx = 0u; idx < m_idx_end; ++idx) {
            std::destroy_at(&m_back->data[idx].val);
        }
        for (auto* ptr = m_front->next; ptr != m_back; ptr = ptr->next) {
            for (auto idx = 0u; idx < chunk_elements; ++idx) {
                std::destroy_at(&ptr->data[idx].val);
            }
        }
        auto* ptr = m_front;
        while (ptr) {
            auto* old = std::exchange(ptr, ptr->next);
            get_allocator().deallocate(old, 1);
        }
    }

    constexpr void push(const T& t) {
        if (!m_back) {
            auto* ptr = get_allocator().allocate(1);
            std::construct_at(ptr);
            m_front = m_back = ptr;
        }
        if (m_idx_end == chunk_elements) {
            auto* ptr = get_allocator().allocate(1);
            std::construct_at(ptr);
            m_back->next = ptr;
            m_back = ptr;
            m_idx_end = 0;
        }
        // TODO: What to do on exception?
        std::construct_at(&m_back->data[m_idx_end].val, t);
        ++m_idx_end;
    }

    constexpr void pop() {
        assert(!empty());
        std::destroy_at(&m_front->data[m_idx_front].val);
        ++m_idx_front;
        if (m_idx_front == chunk_elements) {
            auto* old = std::exchange(m_front, m_front->next);
            get_allocator().deallocate(old, 1);
            m_idx_front = 0;
        }
    }

    constexpr bool empty() const noexcept {
        if (!m_front) return true;
        if (m_front == m_back && m_idx_front == m_idx_end) return true;
        return false;
    }

    constexpr size_t size() const noexcept {
        auto chunks = 0u;
        auto* ptr = m_front;
        while (ptr) {
            ++chunks;
            ptr = ptr->next;
        }
        if (chunks == 0) return 0;
        return (chunks - 1) * chunk_elements + m_idx_end - m_idx_front;
    }

    constexpr size_t length() const noexcept { return size(); }

    constexpr void reserve(size_t new_cap) {}

    size_t capacity() const noexcept {}

 private:
    chunk* m_front{};
    chunk* m_back{};
    index_type m_idx_front{};
    index_type m_idx_end{};
};

}  // namespace upp::experimental
