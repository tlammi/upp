#pragma once

#include <stdexcept>

namespace upp::codec::detail {

class exception : public std::runtime_error {
 public:
    explicit exception(int ec);
    explicit exception(const char* what);
};
}  // namespace upp::codec::detail
