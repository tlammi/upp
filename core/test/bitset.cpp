#include <gtest/gtest.h>

#include <upp/bitset.hpp>

// NOLINTBEGIN(*-magic-numbers)
TEST(Ctor, Default) {
    auto bs = upp::bitset<8>();
    ASSERT_FALSE(bs);
}

TEST(Ctor, Bits) {
    auto bs = upp::bitset<8>{"0b101"};
    ASSERT_EQ(bs[0], true);
    ASSERT_EQ(bs[1], false);
    ASSERT_EQ(bs[2], true);
    ASSERT_EQ(bs[3], false);
    ASSERT_EQ(std::as_const(bs)[0], true);
    ASSERT_EQ(std::as_const(bs)[1], false);
    ASSERT_EQ(std::as_const(bs)[2], true);
    ASSERT_EQ(std::as_const(bs)[3], false);
}

TEST(Ctor, Bytes) {
    auto bs = upp::bitset<16>("0x0f00");
    ASSERT_EQ(bs.byte(0), 0x00);
    ASSERT_EQ(bs.byte(1), 0x0f);
}

// NOLINTEND(*-magic-numbers)
