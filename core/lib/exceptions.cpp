#include <errno.h>

#include <cstring>
#include <stdexcept>
#include <upp/exceptions.hpp>

namespace upp {
void throw_errno(int ec) {
    const char* msg = strerror(ec);
    throw std::runtime_error(msg);
}

void throw_errno() { throw_errno(errno); }

}  // namespace upp
