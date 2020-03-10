#include <gtest/gtest.h>
#include "upp/util.hpp"

TEST(ConvertTest, A) {
    ASSERT_EQ(upp::util::convert<char>(std::string("c")), 'c');
    ASSERT_EQ(upp::util::convert<unsigned char>(std::string("c")), 'c');
    ASSERT_EQ(upp::util::convert<short>(std::string("100")), 100);
    ASSERT_EQ(upp::util::convert<unsigned short>(std::string("100")), 100);
    ASSERT_EQ(upp::util::convert<int>(std::string("100")), 100);
    auto tmp = upp::util::convert<int>("100");
    ASSERT_EQ(tmp, 100);
}