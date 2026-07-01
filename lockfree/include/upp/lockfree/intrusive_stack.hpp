#include <atomic>
#include <upp/lockfree/intrusive_node.hpp>

namespace upp::lockfree {

template <intrusive_node T, class Traits = intrusive_node_traits<T>>
class intrusive_stack;

/**
 * \brief Result of extracting items from a stack
 *
 * This is a thread-unsafe stack received as a result of popping items from a
 * stack.
 * */
template <intrusive_node T, class Traits>
class extracted_stack {
    friend intrusive_stack<T, Traits>;

    std::atomic<T*> m_top{};

    constexpr explicit extracted_stack(T* top) noexcept : m_top(top) {}

 public:
    /**
     * \brief Pop the top element of the stack
     *
     *
     * \return The top element or nullptr if the stack is empty
     * */
    constexpr T* pop() noexcept {
        using enum std::memory_order;
        auto* top = m_top.load(relaxed);
        if (!top) return nullptr;
        auto* next = Traits::next(top).load(relaxed);
        m_top.store(next, relaxed);
        return top;
    }

    /**
     * \brief Reverse the stack
     *
     * This reverses the stack so the bottom element is the new top and top is
     * the new bottom. The complexity is O(n).
     * */
    constexpr void reverse() noexcept {
        // Finally, a place where I need to reverse a linked list
        using enum std::memory_order;
        T* prev = nullptr;
        while (true) {
            auto* ptr = m_top.load(relaxed);
            if (!ptr) return;
            auto* next = Traits::next(ptr).load(relaxed);
            Traits::next(ptr).store(prev, relaxed);
            if (!next) return;
            m_top.store(next, relaxed);
        }
    }

    constexpr auto size() const noexcept {
        std::size_t count = 0;
        using enum std::memory_order;
        auto* ptr = m_top.load(relaxed);
        while (ptr) {
            ++count;
            ptr = Traits::next(ptr).load(relaxed);
        }
        return count;
    }

    [[nodiscard]] constexpr bool empty() const noexcept {
        return m_top.load(std::memory_order::relaxed) == nullptr;
    }
};

/**
 * \brief Lockfree intrusive stack
 *
 * This provides a lockfree stack type leveraging existing storage on the stored
 * elements. No allocation is done, all the references are stored in the stored
 * objects.
 * */
template <intrusive_node T, class Traits>
class intrusive_stack {
    std::atomic<T*> m_top{};

 public:
    /**
     * \brief Push an item to the stack
     *
     * The item must not be part of any other stacks. Otherwise this invokes
     * undefined behavior.
     * */
    constexpr void push(T& t) noexcept {
        using enum std::memory_order;
        auto& next = Traits::next(&t);
        while (true) {
            auto* top = m_top.load(acquire);
            next.store(top, seq_cst);
            if (m_top.compare_exchange_weak(top, &t, release)) return;
        }
    }

    /**
     * \brief Extract all items from a stack
     * */
    constexpr extracted_stack<T, Traits> extract() noexcept {
        auto* top = m_top.exchange(nullptr, std::memory_order::relaxed);
        return extracted_stack<T, Traits>(top);
    }

    /**
     * \brief Check whether the stack is empty
     * */
    [[nodiscard]] constexpr bool empty() const noexcept {
        return m_top.load(std::memory_order::relaxed) == nullptr;
    }
};

}  // namespace upp::lockfree
