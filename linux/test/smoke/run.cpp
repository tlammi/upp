#include <gtest/gtest.h>

#include <upp/linux/run.hpp>


TEST(Simple, True){
  auto res = upp::linux::run("true");
  ASSERT_EQ(res, 0);
}

TEST(Simple, False){
  auto res = upp::linux::run("false");
  ASSERT_EQ(res, 1);
}
