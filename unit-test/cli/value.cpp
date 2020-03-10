#include "upp/cli/value.hpp"
#include <gtest/gtest.h>
#include <limits>

using namespace upp::cli;

namespace util {

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

template <typename T>
void do_integer_test_as() {
    std::string min{stringify(minval<T>())};
    std::string max{stringify(maxval<T>())};
    std::string toosmall{increment(min)};
    std::string toobig{increment(max)};

    ASSERT_EQ(Value{min}.as<T>(), minval<T>());
    ASSERT_EQ(Value{max}.as<T>(), maxval<T>());
    // -1 Is a valid value for an unsigned
    if (minval<T>() != 0)
        ASSERT_THROW(Value{toosmall}.as<T>(), std::out_of_range);
    else
        ASSERT_EQ(Value{"-1"}.as<T>(), maxval<T>());
    ASSERT_THROW(Value{toobig}.as<T>(), std::out_of_range);
}
}  // namespace util

TEST(CliValueTest, AsBool) {
    ASSERT_EQ(Value{"0"}.as<bool>(), false);
    ASSERT_EQ(Value{"1"}.as<bool>(), true);
    ASSERT_EQ(Value{"false"}.as<bool>(), false);
    ASSERT_EQ(Value{"true"}.as<bool>(), true);
    ASSERT_EQ(Value{"FaLse"}.as<bool>(), false);
    ASSERT_EQ(Value{"TrUe"}.as<bool>(), true);
    ASSERT_THROW(Value{"asdfasdf"}.as<bool>(), std::invalid_argument);
}

TEST(CliValueTest, AsChar) {
    for (char c = 32; c < 127; ++c) {
        std::string in{c};
        ASSERT_EQ(Value{in}.as<char>(), c);
    }
    ASSERT_EQ(Value("").as<char>(), '\0');
    ASSERT_THROW(Value("123123").as<char>(), std::invalid_argument);
}

#define DO_TEST_MIN(type)                                       \
    do {                                                        \
        std::string min{util::stringify(util::minval<type>())}; \
        ASSERT_EQ(Value{min}.as<type>(), util::minval<type>()); \
    } while (0)

#define DO_TEST_MAX(type)                                       \
    do {                                                        \
        std::string max{util::stringify(util::maxval<type>())}; \
        ASSERT_EQ(Value{max}.as<type>(), util::maxval<type>()); \
    } while (0)

#define DO_TEST_TOOBIG(type)                                       \
    do {                                                           \
        std::string max{util::stringify(util::maxval<type>())};    \
        std::string toobig{util::increment(max)};                  \
        ASSERT_LT(Value{toobig}.as<type>(), util::maxval<type>()); \
    } while (0)

#define DO_TEST_MINUS1(type) \
    do { ASSERT_EQ(Value{"-1"}.as<type>(), util::maxval<type>()); } while (0)

#define DO_TEST_SET_UNSIGNED(type) \
    do {                           \
        DO_TEST_MIN(type);         \
        DO_TEST_MAX(type);         \
        DO_TEST_MINUS1(type);      \
        DO_TEST_TOOBIG(type);      \
    } while (0)

#define DO_TEST_SET_SIGNED(type) \
    do {                         \
        DO_TEST_MIN(type);       \
        DO_TEST_MAX(type);       \
        DO_TEST_TOOBIG(type);    \
    } while (0)

TEST(CliValueTest, AsShort) {
    DO_TEST_MIN(short);
    DO_TEST_MAX(short);
}

TEST(CliValueTest, AsUShort) { DO_TEST_SET_UNSIGNED(unsigned short); }

TEST(CliValueTest, AsInt) { DO_TEST_SET_SIGNED(int); }

TEST(CliValueTest, AsLongLong) {
    DO_TEST_MIN(long long);
    DO_TEST_MAX(long long);
    std::string tmp{util::stringify(util::minval<long long>())};
    std::string toosmall{util::increment(tmp)};
    ASSERT_THROW(Value{toosmall}.as<long long>(), std::out_of_range);
    tmp = util::stringify(util::maxval<long long>());
    std::string toobig{util::increment(tmp)};
    ASSERT_THROW(Value{toobig}.as<long long>(), std::out_of_range);
}

TEST(CliValueTest, AsULongLong) {
    DO_TEST_MIN(unsigned long long);
    DO_TEST_MAX(unsigned long long);
    ASSERT_EQ(Value{"-1"}.as<unsigned long long>(),
              util::maxval<unsigned long long>());

    std::string tmp{util::stringify(util::maxval<long long>())};
    std::string toobig{util::increment(tmp)};
    ASSERT_THROW(Value{toobig}.as<long long>(), std::out_of_range);
}

TEST(CliValueTest, AsFLoat) { ASSERT_EQ(Value{"0.5"}.as<float>(), 0.5); }