#include <sys/wait.h>
#include <unistd.h>

#include <upp/exceptions.hpp>
#include <upp/linux/process.hpp>
#include <upp/unused.hpp>

namespace upp::linux {
namespace {
constexpr auto to_join_result(int status) noexcept {
    return process::join_result{
        .exit_code = WEXITSTATUS(status),
        .signal = WIFSIGNALED(status) ? WTERMSIG(status) : 0,
    };
}

pid_t do_fork(){
    auto pid = ::fork();
    if (pid < 0) throw_errno();
    return pid;
}
}  // namespace


int run_t::raw_run(const char* path, char* const* argv, char* const* envp, options opts){
  auto pid = do_fork();
  char* const dummy_argv[1] = {nullptr};
  char* const dummy_envp[1] = {nullptr};
  if(!pid){
    (void)::execve(path, dummy_argv, dummy_envp);
    // Execve failed, really nothing that can be done
    ::exit(1);
  } else {
    int exit_code = 0;
    int res = waitpid(pid, &exit_code, 0);
    if(res < 0) throw_errno();
    return WEXITSTATUS(exit_code);
  }
}

pid_t process::do_fork() {
    return upp::linux::do_fork();
}

sbool process::joinable() const noexcept { return m_handle != 0; }

auto process::join() -> join_result {
    assert(joinable() && "Process not joinable");
    int exit_code = 0;
    auto res = waitpid(m_handle, &exit_code, 0);
    if (res < 0) throw_errno();
    m_handle = 0;
    return to_join_result(exit_code);
}

void process::kill(int sig) {
    assert(joinable());
    auto res = ::kill(m_handle, sig);
    if (res < 0) throw_errno();
}

}  // namespace upp::linux
