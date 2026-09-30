#include <unistd.h>

#include <array>
#include <upp/exceptions.hpp>
#include <upp/linux/pipe.hpp>

namespace upp::linux {

pipe_pair pipe() {
    auto fds = std::array<int, 2>();
    int res = pipe2(fds.data(), 0);
    if (res) throw_errno();
    return {
        .read = pipe_read(fds[0]),
        .write = pipe_write(fds[1]),
    };
}

}  // namespace upp::linux
