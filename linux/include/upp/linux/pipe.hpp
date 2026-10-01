#pragma once

#include <linux/limits.h>

#include <upp/fs/file.hpp>
#include <upp/bitmask.hpp>

namespace upp::linux {


enum class pipe_flag: std::uint8_t{
  close_on_exit = 0x1,
  packet = 0x2,
  nonblock = 0x4,
};

class pipe_read : private upp::fs::file {
    using base = upp::fs::file;

 public:
    explicit constexpr pipe_read(int handle) noexcept : base(handle) {}

    using base::read;
};

class pipe_write : private upp::fs::file {
    using base = upp::fs::file;

 public:
    explicit constexpr pipe_write(int handle) noexcept : base(handle) {}

    using base::write;
};

struct pipe_pair {
    pipe_read read;
    pipe_write write;
};

pipe_pair pipe();
pipe_pair pipe(int flags);
pipe_pair pipe(bitmask<pipe_flag> flags);

}  // namespace upp::linux
