#pragma once

#include <upp/fs/file.hpp>

namespace upp::linux {

class pipe_read: private upp::fs::file {
  using base = upp::fs::file;

 public:
    explicit constexpr pipe_read(int handle) noexcept : base(handle) {}

    using base::read;
};

class pipe_write: private upp::fs::file {
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

}  // namespace upp::linux
