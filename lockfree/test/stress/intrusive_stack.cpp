#include <cassert>
#include <list>
#include <thread>
#include <upp/lockfree/intrusive_stack.hpp>
#include <vector>

namespace lf = upp::lockfree;
using namespace std::literals;

struct node {
    std::size_t id{};
    std::atomic<node*> next{};
};

template <>
struct lf::intrusive_node_traits<node> {
    static constexpr std::atomic<node*>& next(node* curr) noexcept {
        return curr->next;
    }
};
class producer {
    size_t m_id{};
    lf::intrusive_stack<node>* m_stack{};
    std::list<node> m_nodes{};

 public:
    producer(size_t id, lf::intrusive_stack<node>& stack)
        : m_id(id), m_stack(&stack) {}

    void operator()(std::stop_token st) {
        while (!st.stop_requested()) {
            m_nodes.emplace_back(m_id, nullptr);
            m_stack->push(m_nodes.back());
        }
    }

    [[nodiscard]] size_t count() const noexcept { return m_nodes.size(); }
    [[nodiscard]] size_t id() const noexcept { return m_id; }

    void reset() { m_nodes.clear(); }
};

int main() {
    auto stack = lf::intrusive_stack<node>();
    auto producers = std::vector<producer>();
    static constexpr auto worker_count = 100uz;
    producers.reserve(worker_count);
    for (size_t i = 0; i < worker_count; ++i) {
        producers.emplace_back(i, stack);
    }

    while (true) {
        auto workers = std::vector<std::jthread>();
        workers.reserve(worker_count);
        for (size_t i = 0; i < worker_count; ++i) {
            workers.emplace_back(
                [&, i](std::stop_token st) { producers.at(i)(st); });
        }
        std::this_thread::sleep_for(4s);
        workers.clear();
        auto extracted = stack.extract();
        auto counts = std::vector<std::size_t>(worker_count);
        while (true) {
            auto* ptr = extracted.pop();
            if (!ptr) break;
            ++counts.at(ptr->id);
        }
        for (size_t i = 0; i < worker_count; ++i) {
            assert(counts[i] == producers[i].count());
        }
        for (auto& p : producers) { p.reset(); }
    }
}
