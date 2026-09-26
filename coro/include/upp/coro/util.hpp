#pragma once

#include <stdexec/execution.hpp>
#include <upp/forward.hpp>

namespace upp::coro {

template <stdexec::scheduler Sched, class Sndr>
constexpr auto run(Sched&& sched, Sndr&& sender) {
  if constexpr(stdexec::sender<Sndr>){
    return stdexec::sync_wait(stdexec::starts_on(UPP_FWD(sched),
                              UPP_FWD(sender)))
        .transform([](auto&& res) {
            if constexpr (std::tuple_size_v<
                              std::remove_cvref_t<decltype(res)>> == 1) {
                return std::get<0>(UPP_FWD(res));
            } else {
                return UPP_FWD(res);
            }
        });
  } else {
    return stdexec::sync_wait(stdexec::schedule(UPP_FWD(sched)) |
                              stdexec::then(UPP_FWD(sender)))
        .transform([](auto&& res) {
            if constexpr (std::tuple_size_v<
                              std::remove_cvref_t<decltype(res)>> == 1) {
                return std::get<0>(UPP_FWD(res));
            } else {
                return UPP_FWD(res);
            }
        });

  }
}
}  // namespace upp::coro
