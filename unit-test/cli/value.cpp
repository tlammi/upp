#include "upp/cli/value.hpp"
#include <gtest/gtest.h>
#include <limits>

using namespace upp::cli;

TEST(ValueTest, As) { ASSERT_EQ(Value{"-1"}.as<int>(), -1); }
