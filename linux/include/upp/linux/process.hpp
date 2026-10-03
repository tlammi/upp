#pragma once

#include <sys/types.h>

#include <functional>
#include <upp/cstr.hpp>
#include <upp/forward.hpp>
#include <upp/sbool.hpp>
#include <upp/linux/pipe.hpp>
#include <utility>

namespace upp::linux {

namespace detail{
template<class T>
concept c_stringable = requires(T& t){
  {t.c_str()} -> std::convertible_to<const char*>;
} || std::convertible_to<T, const char*>;

}
class run_t{

  template<class T>
    static constexpr const char* to_c_str(T& t) noexcept {
      if constexpr (std::convertible_to<T, const char*>)
        return t;
      else return t.c_str();
    }

  struct options{
    pipe_read stdin{};
    pipe_write stdout{};
    pipe_write stderr{};
  };

  static int raw_run(const char* path, char* const* argv, char* const* envp, options opts);


  public:
  constexpr explicit run_t() noexcept = default;

  template<detail::c_stringable T>
  int operator()(T& t) const {
    return raw_run(to_c_str(t), nullptr, nullptr, {});
  }

};


constexpr run_t run{};

class process {
    pid_t m_handle{};

    static pid_t do_fork();

 public:
    constexpr process() noexcept = default;

    template <class Fn, class... Ts>
    process(Fn&& fn, Ts&&... ts)
        requires(std::invocable<Fn, Ts...>)
        : m_handle(do_fork()) {
        using result_type = std::invoke_result_t<Fn, Ts...>;
        if (!m_handle) {
            if constexpr (std::same_as<result_type, void>) {
                std::invoke(UPP_FWD(fn), UPP_FWD(ts)...);
                ::exit(0);
            } else {
                ::exit(std::invoke(UPP_FWD(fn), UPP_FWD(ts)...));
            }
        }
    }

    process(const process&) = delete;
    process& operator=(const process&) = delete;

    process(process&& other) noexcept
        : m_handle(std::exchange(other.m_handle, 0)) {}
    process& operator=(process&& other) noexcept {
        std::destroy_at(this);
        std::construct_at(this, std::move(other));
        return *this;
    }

    constexpr ~process() { assert(!joinable()); }

    sbool joinable() const noexcept;

    struct join_result {
        int exit_code;
        int signal;

        constexpr sbool ok() const noexcept {
            return exit_code == 0 && signal == 0;
        }
    };

    join_result join();

    void kill(int sig);
};

}  // namespace upp::linux
