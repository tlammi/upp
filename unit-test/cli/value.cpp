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
    std::cout << "stringified: " << out << std::endl;
    return out;
}

template <typename T>
void do_integer_test_as() {
    long long min{minval<T>()};
    long long max{maxval<T>()};
    long long toosmall{min - 1};
    long long toobig{max + 1};

    ASSERT_EQ(Value{stringify(min)}.as<T>(), min);
    ASSERT_EQ(Value{stringify(max)}.as<T>(), max);

    ASSERT_THROW(Value{stringify(toosmall)}.as<T>(), std::out_of_range);
    ASSERT_THROW(Value{stringify(toobig)}.as<T>(), std::out_of_range);
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

TEST(CliValueTest, AsShort) { util::do_integer_test_as<short>(); }
TEST(CliValueTest, AsInt) { util::do_integer_test_as<int>(); }

TEST(CliValueTest, AsUShort) { util::do_integer_test_as<unsigned short>(); }
TEST(CliValueTest, AsUInt) { util::do_integer_test_as<unsigned int>(); }