#pragma once

#include <stdexec/execution.hpp>

namespace upp::coro {
class yield_t {
    template <class R>
    struct op {
        R r;
        void start() & noexcept { stdexec::set_value(std::move(r)); }
    };

    struct yield_sender {
        using sender_concept = stdexec::sender_t;
        using completion_signatures =
            stdexec::completion_signatures<stdexec::set_value_t()>;

        template <stdexec::receiver_of<completion_signatures> Recv>
        op<Recv> connect(Recv recv) noexcept(
            std::is_nothrow_move_constructible_v<Recv>) {
            return op{std::move(recv)};
        }
    };

 public:
    constexpr auto operator()() const noexcept { return yield_sender{}; }
};

constexpr yield_t yield{};
}  // namespace upp::coro
