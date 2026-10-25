#include <gtest/gtest.h>

#include <upp/matcher.hpp>
#include <upp/unused.hpp>

using upp::all_of;
using upp::any_of;
using upp::none_of;

TEST(OneTrue, AllOf) {
    auto r = 1 == all_of(1);
    ASSERT_TRUE(r);
}

TEST(OneTrue, AnyOf) {
    auto r = 1 == any_of(1);
    ASSERT_TRUE(r);
}

TEST(OneTrue, NoneOf) {
    auto r = 1 == none_of(2);
    ASSERT_TRUE(r);
}

TEST(OneFalse, AllOf) {
    auto r = 1 == all_of(2);
    ASSERT_FALSE(r);
}

TEST(OneFalse, AnyOf) {
    auto r = 1 == any_of(2);
    ASSERT_FALSE(r);
}

TEST(OneFalse, NoneOf) {
    auto r = 1 == none_of(1);
    ASSERT_FALSE(r);
}

TEST(AllOf, MatchOne) {
    auto r = 1 == all_of(1, 2);
    ASSERT_FALSE(r);
}

TEST(AllOf, MatchAll) {
    auto r = 1 == all_of(1, 1, 1, 1, 1);
    ASSERT_TRUE(r);
}

TEST(AnyOf, MatchOne) {
    auto r = 1 == any_of(1, 2);
    ASSERT_TRUE(r);
}

TEST(AnyOf, MatchNone) {
    auto r = 1 == any_of(2, 3, 4, 5, 6);
    ASSERT_FALSE(r);
}

TEST(NoneOf, MatchOne) {
    auto r = 1 == none_of(1, 2);
    ASSERT_FALSE(r);
}

TEST(NoneOf, MatchNone) {
    auto r = 1 == none_of(2, 3, 4, 5, 6);
    ASSERT_TRUE(r);
}
