#include <gtest/gtest.h>

#include <upp/fs/search_path.hpp>

TEST(Simple, True) {
    auto res = upp::fs::search_path("true");
    ASSERT_FALSE(res.empty());
    ASSERT_TRUE(res.native().starts_with("/"));
}
