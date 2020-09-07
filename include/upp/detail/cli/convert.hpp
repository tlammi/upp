#pragma once

#include <stdexcept>
#include <string>
namespace upp {
namespace cli {

template <typename T>
T convert_to(const char* str);

}  // namespace cli
}  // namespace upp