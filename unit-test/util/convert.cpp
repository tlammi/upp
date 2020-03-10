#include <gtest/gtest.h>
#include "upp/util.hpp"

using namespace upp::util;

template <typename T>
constexpr T minval() {
    return std::numeric_limits<T>::min();
}

template <typename T>
constexpr T maxval() {
    return std::numeric_limits<T>::max();
}

template <typename T>
std::string stringify(const T& in) {
    std::stringstream ss;
    ss << in;
    std::string out;
    ss >> out;
    return out;
}

static std::string increment(std::string in) {
    if (in == "0") return "-1";
    bool done{false};
    for (int i = in.size() - 1; i >= 0; i++) {
        size_t index = static_cast<size_t>(i);
        if (in[index] == '9') {
            in[index] = '0';
        } else {
            ++in[index];
            done = true;
            break;
        }
    }
    if (!done) {
        if (!(in[0] == '-'))
            in.insert(0, "1");
        else
            in.insert(1, "1");
    }
    std::cout << "incremented: " << in << std::endl;
    return in;
}

#define DO_TEST_MIN(type)                              \
    do {                                               \
        std::string min{stringify(minval<type>())};    \
        ASSERT_EQ(convert<type>(min), minval<type>()); \
    } while (0)

#define DO_TEST_MAX(type)                              \
    do {                                               \
        std::string max{stringify(maxval<type>())};    \
        ASSERT_EQ(convert<type>(max), maxval<type>()); \
    } while (0)

#define DO_TEST_TOOBIG(type)                              \
    do {                                                  \
        std::string max{stringify(maxval<type>())};       \
        std::string toobig{increment(max)};               \
        ASSERT_LT(convert<type>(toobig), maxval<type>()); \
    } while (0)

#define DO_TEST_TOOSMALL(type)                              \
    do {                                                    \
        std::string tmp{stringify(minval<type>())};         \
        tmp = increment(tmp);                               \
        std::string toosmall{increment(tmp)};               \
        ASSERT_GT(convert<type>(toosmall), minval<type>()); \
    } while (0)

#define DO_TEST_MINUS1(type) \
    do { ASSERT_EQ(convert<type>("-1"), maxval<type>()); } while (0)

TEST(ConvertTest, Max) {
    DO_TEST_MAX(short);
    DO_TEST_MAX(int);
    DO_TEST_MAX(long);
    DO_TEST_MAX(long long);
    DO_TEST_MAX(unsigned short);
    DO_TEST_MAX(unsigned int);
    DO_TEST_MAX(unsigned long);
    DO_TEST_MAX(unsigned long long);
}

TEST(ConvertTest, Min) {
    DO_TEST_MIN(short);
    DO_TEST_MIN(int);
    DO_TEST_MIN(long);
    DO_TEST_MIN(long long);
    DO_TEST_MIN(unsigned short);
    DO_TEST_MIN(unsigned int);
    DO_TEST_MIN(unsigned long);
    DO_TEST_MIN(unsigned long long);
}

TEST(ConvertTest, TooBig) {
    DO_TEST_TOOBIG(short);
    DO_TEST_TOOBIG(int);
    DO_TEST_TOOBIG(unsigned short);
    DO_TEST_TOOBIG(unsigned int);

    std::string tmp{stringify(maxval<long long>())};
    tmp = increment(tmp);
    ASSERT_THROW(convert<long long>(tmp), std::out_of_range);
    ASSERT_THROW(convert<short>(tmp), std::out_of_range);
    tmp = stringify(maxval<unsigned long long>());
    tmp = increment(tmp);
    ASSERT_THROW(convert<unsigned long long>(tmp), std::out_of_range);
    ASSERT_THROW(convert<unsigned short>(tmp), std::out_of_range);
}

TEST(ConvertTest, TooSmall) {
    DO_TEST_TOOSMALL(short);
    DO_TEST_TOOSMALL(int);
    DO_TEST_TOOSMALL(long);
    std::string tmp{stringify(minval<long long>())};
    tmp = increment(tmp);
    ASSERT_THROW(convert<long long>(tmp), std::out_of_range);
}

TEST(ConvertTest, UnsignedMinus1) {
    DO_TEST_MINUS1(unsigned short);
    DO_TEST_MINUS1(unsigned int);
    DO_TEST_MINUS1(unsigned long);
    DO_TEST_MINUS1(unsigned long long);
}