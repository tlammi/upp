#include "upp/cli/value.hpp"

namespace upp {
namespace cli {

Value::Value() : Value("") {}
Value::Value(std::string value) : _value{value} {}

template <>
std::string Value::as() const {
    return _value;
}

template <>
const char* Value::as() const {
    return _value.c_str();
}

#define AS_SPECIALIZATION(type) \
    template <>                 \
    type Value::as() const {    \
        std::stringstream ss;   \
        ss << _value;           \
        type out;               \
        ss >> out;              \
        return out;             \
    }

AS_SPECIALIZATION(bool)
AS_SPECIALIZATION(char)
AS_SPECIALIZATION(short)
AS_SPECIALIZATION(int)
AS_SPECIALIZATION(long)
AS_SPECIALIZATION(long long)
AS_SPECIALIZATION(unsigned char)
AS_SPECIALIZATION(unsigned short)
AS_SPECIALIZATION(unsigned int)
AS_SPECIALIZATION(unsigned long)
AS_SPECIALIZATION(unsigned long long)
AS_SPECIALIZATION(float)
AS_SPECIALIZATION(double)
AS_SPECIALIZATION(long double)

Value& Value::operator=(const std::string& value) {
    _value = value;
    return *this;
}
}  // namespace cli
}  // namespace upp
