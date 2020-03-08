#include "upp/cli/value.hpp"
#include <gtest/gtest.h>

TEST(CliValueTest, A) {
    upp::cli::Value v{"100"};
    ASSERT_EQ(v.as<int>(), 100);
    upp::cli::Value v0{"1"};
    ASSERT_EQ(v.as<bool>(), true);
    ASSERT_EQ(upp::cli::Value{"0"}.as<bool>(), false);
}