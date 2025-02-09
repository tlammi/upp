#include <gtest/gtest.h>

#include <upp/ranges.hpp>

TEST(Range, Empty) {
    for (auto _ : upp::range(0)) { GTEST_FAIL(); }
}

TEST(Range, Small) {
    static constexpr size_t count = 100;
    auto v = upp::range(count) | std::ranges::to<std::vector<int>>();
    ASSERT_EQ(v.size(), count);
    ASSERT_EQ(v[0], 0);
    ASSERT_EQ(v[v.size() - 1], count - 1);
}
