#pragma once

#include <linux/limits.h>

#include <upp/bitmask.hpp>
#include <upp/fs/file.hpp>

namespace upp::linux {

enum class pipe_flag : std::uint8_t {
    close_on_exit = 0x1,
    packet = 0x2,
    nonblock = 0x4,
};

struct pipe_pair {
    upp::fs::readable_file read;
    upp::fs::writable_file write;
};

pipe_pair pipe();
pipe_pair pipe(int flags);
pipe_pair pipe(bitmask<pipe_flag> flags);

}  // namespace upp::linux
