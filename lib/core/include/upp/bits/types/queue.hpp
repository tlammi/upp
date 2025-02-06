#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>

namespace upp {
namespace queue_detail {

struct NodeHeader {
    NodeHeader* next{};
    NodeHeader* prev{};
};

constexpr size_t CHUNK_SIZE_HINT = 1024;
template <class T>
constexpr size_t chunk_elems() {
    const auto v = CHUNK_SIZE_HINT / sizeof(T);
    return v ? v : 1;
}
}  // namespace queue_detail

template <class T>
class Queue {
    using NodeHeader = queue_detail::NodeHeader;
    union Union {
        std::uint8_t unused{};
        T val;
    };
    static constexpr size_t chunk_size = queue_detail::chunk_elems<T>();

    struct Node : NodeHeader {
        std::array<Union, chunk_size> data{};
    };

    Node* m_front{};   // First node in the list
    size_t m_start{};  // index where the first active element is
    size_t m_stop{};   // index one past where the last active element is

    static Node* node_cast(NodeHeader* hdr) { return static_cast<Node*>(hdr); }
    constexpr Node* last_node() const noexcept {
        if (!m_front->prev) return m_front;
        return node_cast(m_front->prev);
    }

    // ensure that the nodes exist
    void bootstrap() {
        if (m_front) return;
        m_front = new Node();  // NOLINT
        m_front->prev = m_front;
    }

    void reserve_back(size_t count) {
        bootstrap();
        const auto left = chunk_size - m_stop;
        if (count <= left) return;
        count -= left;
        auto* node = last_node();
        while (count > chunk_size) {
            if (!node->next) {
                node->next = new Node();
                node->next->prev = node;
            }
            node = node_cast(node->next);
            count -= chunk_size;
        }
        if (!node->next) {
            node->next = new Node();
            node->next->prev = node;
        }
    }

 public:
    class iterator {
        friend class Queue;
        Node* m_node{};
        size_t m_idx{};

        constexpr iterator(Node* node, size_t idx) : m_node(node), m_idx(idx) {}

     public:
        using value_type = T;
        using reference = T&;
        using pointer_type = T*;
        using difference_type = std::ptrdiff_t;

        constexpr iterator() = default;
        constexpr bool operator==(const iterator& other) const noexcept {
            if (m_node != other.m_node) return false;
            if (m_idx != other.m_idx) return false;
            return true;
        }

        constexpr reference operator*() const noexcept {
            return m_node->data[m_idx].val;
        }

        constexpr pointer_type operator->() const noexcept {
            return &m_node->data[m_idx].val;
        }

        constexpr iterator& operator++() noexcept {
            ++m_idx;
            if (m_idx == chunk_size) {
                m_idx = 0;
                m_node = node_cast(m_node->next);
            }
            return *this;
        }
        constexpr iterator operator++(int) {
            auto cpy = *this;
            ++*this;
            return cpy;
        }
    };

    using size_type = size_t;

    constexpr Queue() = default;

    constexpr explicit Queue(std::initializer_list<T> t) {
        reserve_back(t.size());
        auto* node = m_front;
        for (auto&& v : t) {
            node->data[m_stop].val = std::move(v);
            ++m_stop;
            if (m_stop == chunk_size) {
                node = node_cast(node->next);
                m_stop = 0;
            }
        }
    }
    // TODO: implement
    Queue(const Queue&) = delete;
    // TODO: implement
    Queue& operator=(const Queue&) = delete;

    constexpr Queue(Queue&& other) noexcept
        : m_front(std::exchange(other.m_front, nullptr)),
          m_start(std::exchange(other.m_start, 0)),
          m_stop(std::exchange(other.m_stop, 0)) {}

    Queue& operator=(Queue&& other) noexcept {
        auto tmp = Queue(std::move(other));
        std::swap(m_front, other.m_front);
        std::swap(m_start, other.m_start);
        std::swap(m_stop, other.m_stop);
        return *this;
    }

    constexpr ~Queue() {
        if (!m_front) return;
        auto* node = m_front;
        auto* last = last_node();
        while (node != last) {
            for (size_t i = m_start; i < chunk_size; ++i) {
                node->data[i].val.~T();
            }
            node = node_cast(node->next);
            m_start = 0;
        }
        for (size_t i = m_start; i < m_stop; ++i) { node->data[i].val.~T(); }
    }

    iterator begin() { return {m_front, m_start}; }
    iterator end() { return {m_front ? last_node() : nullptr, m_stop}; }

    constexpr size_type size() const noexcept {
        if (!m_front) return 0;
        const auto* node = m_front;
        const auto* end = last_node();
        size_t chunks = 1;
        while (node != end) {
            ++node;
            ++chunks;
        }
        return (chunks - 1) * chunk_size + (m_stop - m_start);
    }

    constexpr size_type length() const noexcept { return size(); }

    constexpr bool empty() const noexcept {
        return !m_front || (m_front == last_node() && m_stop == m_start);
    }

    template <class... Ts>
    void emplace_back(Ts&&... ts) {
        if (!m_front) {
            m_front = new Node();
            m_front->prev = m_front;
        } else if (m_stop == chunk_size) {
            auto* last = last_node();
            if (last->next) {
                last = node_cast(last->next);
            } else {
                last->next = new Node();
                last->next->prev = last;
                last = node_cast(last->next);
            }
            m_front->prev = last;
            m_stop = 0;
        }

        std::construct_at(&last_node()->data[m_stop].val,
                          std::forward<Ts>(ts)...);
        ++m_stop;
    }
};
}  // namespace upp
