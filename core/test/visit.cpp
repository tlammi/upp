#include <gtest/gtest.h>

#include <upp/visit.hpp>

TEST(Visit, First) {
    auto v = std::variant<int, double>(1);
    auto res = upp::visit(v, [](int i) { return i; }, [](double) { return 0; });
    ASSERT_EQ(res, 1);
}

TEST(Visit, Second) {
    auto v = std::variant<int, double>(1.0);
    auto res = upp::visit(
        v,
        [](int i) {
            (void)i;
            return 0;
        },
        [](double d) { return static_cast<int>(d); });

    ASSERT_EQ(res, 1);
}
