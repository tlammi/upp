#include "upp/util.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>

#define SPECIALIZE_STR_TO_NUMBER(outtype, tmptype, convfunc)             \
    template <>                                                          \
    outtype convert(const std::string& in) {                             \
        size_t lastindex;                                                \
        tmptype tmp{convfunc(in, &lastindex, 0)};                        \
        if (lastindex != in.size())                                      \
            throw std::invalid_argument("Not a number: \"" + in + "\""); \
        return static_cast<outtype>(tmp);                                \
    }

#define SPECIALIZE_STR_TO_UNSIGNED(outtype) \
    SPECIALIZE_STR_TO_NUMBER(outtype, unsigned long long, std::stoull)

#define SPECIALIZE_STR_TO_SIGNED(outtype) \
    SPECIALIZE_STR_TO_NUMBER(outtype, long long, std::stoll)

#define SPECIALIZE_STR_TO_FLOATING(outtype)                              \
    template <>                                                          \
    outtype convert(const std::string& in) {                             \
        size_t lastindex;                                                \
        long double tmp{std::stold(in, &lastindex)};                     \
        if (lastindex != in.size())                                      \
            throw std::invalid_argument("Not a number: \"" + in + "\""); \
        return static_cast<outtype>(tmp);                                \
    }

namespace upp {
namespace util {

template <>
bool convert(const std::string& in) {
    if (in == "0") return false;
    if (in == "1") return true;
    std::string tmp{in};
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
    if (tmp == "false") return false;
    if (tmp == "true") return true;
    throw std::invalid_argument("Could not convert value to boolean: " + in);
}

template <>
char convert(const std::string& in) {
    if (in.size() > 1)
        throw std::invalid_argument("Could not value argument to char: " + in);
    if (in.size() == 1) return in[0];
    return '\0';
}

template <>
unsigned char convert(const std::string& in) {
    return static_cast<unsigned char>(convert<char>(in));
}

SPECIALIZE_STR_TO_SIGNED(short);
SPECIALIZE_STR_TO_UNSIGNED(unsigned short);
SPECIALIZE_STR_TO_SIGNED(int);
SPECIALIZE_STR_TO_UNSIGNED(unsigned int);
SPECIALIZE_STR_TO_SIGNED(long);
SPECIALIZE_STR_TO_UNSIGNED(unsigned long);
SPECIALIZE_STR_TO_SIGNED(long long);
SPECIALIZE_STR_TO_UNSIGNED(unsigned long long);
SPECIALIZE_STR_TO_FLOATING(float);
SPECIALIZE_STR_TO_FLOATING(double);
SPECIALIZE_STR_TO_FLOATING(long double);

template <>
std::string convert(const std::string& in) {
    return in;
}

template <>
const char* convert(const std::string& in) {
    return in.c_str();
}

}  // namespace util
}  // namespace upp
