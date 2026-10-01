#pragma once

#include <upp/cstr.hpp>
#include <upp/sbool.hpp>
#include <utility>
#include <sys/types.h>
#include <upp/forward.hpp>
#include <functional>

namespace upp::linux {

int run(const char* path, char* const argv[],  char* const envp[]);

class process{
  pid_t m_handle{};

  static pid_t do_fork();
public:
  constexpr process() noexcept = default;

  template<class Fn, class... Ts>
  process(Fn&& fn, Ts&&... ts) requires(std::invocable<Fn, Ts...>){
    using result_type = std::invoke_result_t<Fn, Ts...>;
    m_handle = do_fork();
    if(!m_handle) {
      if constexpr (std::same_as<result_type, void>){
      std::invoke(UPP_FWD(fn), UPP_FWD(ts)...);
      ::exit(0);
      }
      else  {
      ::exit(std::invoke(UPP_FWD(fn), UPP_FWD(ts)...));
      }
    }
  }

  process(const process&) = delete;
  process& operator=(const process&) = delete;

  process(process&& other) noexcept : m_handle(std::exchange(other.m_handle, 0)){}
  process& operator=(process&& other) noexcept {
    std::destroy_at(this);
    std::construct_at(this, std::move(other));
    return *this;
  }

  constexpr ~process() {
    assert(!joinable());
  }

  sbool joinable() const noexcept;

  int join();
};

}
