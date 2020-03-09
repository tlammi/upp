#include "upp/cli/value.hpp"

#include <algorithm>
#include <iostream>
#include <limits>

namespace upp {
namespace cli {

template <typename T1, typename T2>
static inline bool fits(T2 val) {
    return val <= std::numeric_limits<T1>::max() &&
           val >= std::numeric_limits<T1>::min();
}

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

template <>
bool Value::as() const {
    if (_value == "0") return false;
    if (_value == "1") return true;
    std::string tmp{_value};
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
    if (tmp == "false") return false;
    if (tmp == "true") return true;
    throw std::invalid_argument("Could not convert value to boolean: " +
                                _value);
}

template <>
char Value::as() const {
    if (_value.size() > 1)
        throw std::invalid_argument("Could not value argument to char: " +
                                    _value);
    if (_value.size() == 1) return _value[0];
    return '\0';
}

template <>
short Value::as() const {
    int tmp{std::stoi(_value, nullptr, 0)};
    if (fits<short>(tmp)) return static_cast<short>(tmp);
    throw std::out_of_range("Number does not fit in short: " + _value);
}

template <>
int Value::as() const {
    return std::stoi(_value, nullptr, 0);
}

template <>
unsigned short Value::as() const {
    unsigned long tmp{std::stoul(_value, nullptr, 0)};
    if (fits<unsigned short>(tmp)) return static_cast<unsigned short>(tmp);
    if (_value == "-1") return static_cast<unsigned short>(tmp);
    throw std::out_of_range("Number does not fit in unsigned short: " + _value);
}

template <>
unsigned int Value::as() const {
    unsigned long long tmp{std::stoull(_value, nullptr, 0)};
    if (fits<unsigned int>(tmp)) return static_cast<unsigned int>(tmp);
    if (_value == "-1") return static_cast<unsigned int>(tmp);
    throw std::out_of_range("Number does not fit in unsigned int: " + _value);
}

#define SPECIALIZE(type, func)           \
    template <>                          \
    type Value::as() const {             \
        return func(_value, nullptr, 0); \
    }

SPECIALIZE(long, std::stol);
SPECIALIZE(long long, std::stoll);

SPECIALIZE(unsigned long, std::stoul);
SPECIALIZE(unsigned long long, std::stoull);
AS_SPECIALIZATION(unsigned char)

AS_SPECIALIZATION(float)
AS_SPECIALIZATION(double)
AS_SPECIALIZATION(long double)

Value& Value::operator=(const std::string& value) {
    _value = value;
    return *this;
}
}  // namespace cli
}  // namespace upp
