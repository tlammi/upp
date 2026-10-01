#include <upp/linux/process.hpp>
#include <unistd.h>
#include <upp/exceptions.hpp>
#include <sys/wait.h>

namespace upp::linux{

pid_t process::do_fork() {
  auto pid = ::fork();
  if(pid < 0) throw_errno();
  return pid;
}


sbool process::joinable() const noexcept {
  return m_handle != 0;
}

int process::join() {
  assert(joinable() && "Process not joinable");
  int exit_code = 0;
  auto res = waitpid(m_handle, &exit_code, 0);
  if(res < 0) throw_errno();
  m_handle = 0;
  return WEXITSTATUS(exit_code);
}

}
