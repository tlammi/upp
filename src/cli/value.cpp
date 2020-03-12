#include "upp/cli/value.hpp"

#include <algorithm>
#include <iostream>
#include <limits>

namespace upp {
namespace cli {

Value::Value() : Value("") {}
Value::Value(const std::string& value) : _value{value} {}

Value& Value::operator=(const std::string& value) {
    _value = value;
    return *this;
}
}  // namespace cli
}  // namespace upp
