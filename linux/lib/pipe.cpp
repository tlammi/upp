#include <fcntl.h>
#include <unistd.h>

#include <array>
#include <upp/exceptions.hpp>
#include <upp/linux/pipe.hpp>

namespace upp::linux {
namespace {

constexpr int pipe_flag_to_native(bitmask<pipe_flag> flags) noexcept {
    int out = 0;
    using enum pipe_flag;
    if (flags & close_on_exit) out |= O_CLOEXEC;
    if (flags & packet) out |= O_DIRECT;
    if (flags & nonblock) out |= O_NONBLOCK;
    return out;
}

}  // namespace

pipe_pair pipe() { return pipe(0); }

pipe_pair pipe(int flags) {
    auto fds = std::array<int, 2>();
    int res = pipe2(fds.data(), flags);
    if (res) throw_errno();
    return {
        .read = fs::readable_file(fds[0]),
        .write = fs::writable_file(fds[1]),
    };
}
pipe_pair pipe(bitmask<pipe_flag> flags) {
    return pipe(pipe_flag_to_native(flags));
}

}  // namespace upp::linux
