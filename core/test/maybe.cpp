#include <gtest/gtest.h>

#include <upp/maybe.hpp>

using namespace std::literals;

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

TEST(Ctor, StrViewNull) {
    auto m = upp::maybe<std::string_view>();
    ASSERT_FALSE(m);
}

TEST(Ctor, StrViewEmpty) {
    auto m = upp::maybe(""sv);
    ASSERT_TRUE(m);
    ASSERT_EQ(*m, "");
    m = upp::maybe(std::string_view());
    ASSERT_TRUE(m);
    ASSERT_EQ(*m, "");
}

TEST(Ctor, SpanNull) {
    static constexpr auto m = upp::maybe<std::span<int>>();
    ASSERT_FALSE(m);
}

TEST(Ctor, SpanEmpty) {
    static constexpr auto m = upp::maybe(std::span<const int>());
    ASSERT_TRUE(m);
}

TEST(AsOptional, Value) {
    auto m = upp::maybe<std::size_t>(1);
    auto o = m.as_optional();
    ASSERT_TRUE(o);
    ASSERT_EQ(*o, 1);
}

TEST(AsOptional, NoValue) {
    auto m = upp::maybe<std::size_t>();
    auto o = m.as_optional();
    ASSERT_FALSE(o);
}
