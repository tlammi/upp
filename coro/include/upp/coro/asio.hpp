#pragma once

#include <boost/asio.hpp>
#include <stdexec/execution.hpp>
#include <upp/forward.hpp>

namespace upp::coro {

template <class Ctx>
class asio_scheduler {
    Ctx* m_ctx;

 public:
    explicit constexpr asio_scheduler(Ctx& ctx) noexcept : m_ctx(&ctx) {}

    template <class R>
    class op {
        R m_recv;
        Ctx* m_ctx;

     public:
        op(R r, Ctx& ctx) noexcept : m_recv(std::move(r)), m_ctx(&ctx){}

        constexpr void start() & noexcept {
          boost::asio::post(*m_ctx, [r = std::move(m_recv)] mutable noexcept {
              stdexec::set_value(std::move(r));
              });
        }
    };

    class sender {
        Ctx* m_ctx;

     public:
        using sender_concept = stdexec::sender_tag;
        using completion_signatures = stdexec::completion_signatures<stdexec::set_value_t()>;
        explicit constexpr sender(Ctx& ctx) noexcept : m_ctx(&ctx) {}

        constexpr auto connect(auto&& receiver) const& noexcept {
          return op{UPP_FWD(receiver), *m_ctx};
        }
    };

    constexpr auto schedule() const noexcept { return sender{*m_ctx}; }

    constexpr auto operator<=>(const asio_scheduler&) const noexcept = default;
};

}  // namespace upp::coro
