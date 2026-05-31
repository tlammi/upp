#include <gtest/gtest.h>

#include <upp/maybe.hpp>

TEST(Ctor, PtrNoVal) {
    auto m = upp::maybe<int*>();
    ASSERT_FALSE(m);
}

TEST(Ctor, PtrVal) {
    int i = 0;
    auto m = upp::maybe(&i);
    ASSERT_TRUE(m);
}

TEST(Ctor, FromNull) {
    auto lambda = []() -> upp::maybe<std::size_t> { return upp::null_maybe; };
    ASSERT_FALSE(lambda());
}
