#include <unistd.h>

#include <upp/exceptions.hpp>
#include <upp/linux/run.hpp>

namespace upp::linux {

void run_t::execute(const char* path, char* const* argv, char* const* envp) {
    (void)::execvpe(path, argv, envp);
    // execvpe never returns on success.
    throw_errno();
}

}  // namespace upp::linux
