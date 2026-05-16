#include <gtest/gtest.h>

#include <upp/bitmask.hpp>

template <upp::enum_type E>
using bitmask = upp::bitmask<E>;

enum class sample : uint8_t {
    a = 1,
    b = 1 << 1,
    c = 1 << 2,
    d = 1 << 3,
};

static_assert(bitmask<sample>().none());
static_assert(!bitmask<sample>().any());

TEST(Bits, None) {
    auto b = upp::bitmask<sample>();
    ASSERT_TRUE(b.none());
    ASSERT_FALSE(b.any());
}

TEST(Bits, One) {
    auto b = upp::bm | sample::a;
    ASSERT_FALSE(b.none());
    ASSERT_TRUE(b.any());
}

TEST(Bits, Some) {
    using enum sample;
    auto bm = upp::bm | a | b | c;
    ASSERT_FALSE(bm.none());
    ASSERT_TRUE(bm.any());
}

TEST(Bits, All) {
    using enum sample;
    auto bm = upp::bm | a | b | c | d;
    ASSERT_FALSE(bm.none());
    ASSERT_TRUE(bm.any());
}

static_assert(!bitmask<sample>().all());

TEST(All, None) {
    auto b = upp::bitmask<sample>();
    ASSERT_FALSE(b.all());
}

TEST(All, One) {
    auto b = upp::bm | sample::a;
    ASSERT_FALSE(b.all());
}

TEST(All, Some) {
    using enum sample;
    auto bm = upp::bm | a | b | c;
    ASSERT_FALSE(bm.all());
}

TEST(All, All) {
    using enum sample;
    auto bm = upp::bm | a | b | c | d;
    ASSERT_TRUE(bm.all());
}
