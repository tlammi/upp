#include <gtest/gtest.h>

#include <upp/linux/run.hpp>

TEST(Simple, True) {
    auto res = upp::linux::run("true");
    ASSERT_EQ(res, 0);
}

TEST(Simple, False) {
    auto res = upp::linux::run("false");
    ASSERT_EQ(res, 1);
}

TEST(Simple, Args) {
    auto res = upp::linux::run("sh", "-c", "exit 2");
    ASSERT_EQ(res, 2);
}

TEST(Simple, Dynamic){
  auto args = std::vector<std::string>{"sh", "-c", "exit 3"};
  auto res = upp::linux::run(args);
  ASSERT_EQ(res, 3);
}
/*
TEST(Io, Stdout){
  auto pipe = upp::linux::pipe();

  auto res = upp::linux::run("echo","foo", {.std_out = pipe.write});
}
*/
