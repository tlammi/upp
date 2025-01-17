#include <gtest/gtest.h>

#include <upp/enum_array.hpp>

enum enum1 {
    val1,
    val2,
    val3,
};

TEST(Init, Default) {
    auto a = upp::enum_array<int, enum1>();
    ASSERT_EQ(a.size(), upp::enum_count<enum1>());
}

TEST(Init, Values) {
    auto a = upp::enum_array<int, enum1>{1, 2, 3};
    using enum enum1;
    ASSERT_EQ(a[val1], 1);
    ASSERT_EQ(a[val2], 2);
    ASSERT_EQ(a[val3], 3);
}

TEST(Init, MakeEnumArr) {
    using enum enum1;

    auto a = upp::make_enum_array<int, enum1>({
        {val2, 2},
        {val1, 1},
        {val3, 3},
    });

    ASSERT_EQ(a[val1], 1);
    ASSERT_EQ(a[val2], 2);
    ASSERT_EQ(a[val3], 3);
}
