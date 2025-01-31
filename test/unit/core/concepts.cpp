#include <gtest/gtest.h>

#include <upp/concepts.hpp>

int foo() { return 1; }
static_assert(upp::concepts::prototype<decltype(foo), int()>);
static_assert(upp::concepts::prototype<decltype(&foo), int()>);
static_assert(!upp::concepts::prototype<decltype(foo), void()>);
static_assert(!upp::concepts::prototype<decltype(foo), int(int)>);

void bar() {}
static_assert(upp::concepts::prototype<decltype(bar), void()>);

static_assert(upp::concepts::prototype<decltype([] {}), void()>);
static_assert(!upp::concepts::prototype<decltype([] {}), int()>);
static_assert(!upp::concepts::prototype<decltype([] {}), void(int)>);

enum class BitMask {
    A = 1,
    B = 1 << 1,
    C = 1 << 2,
};

static_assert(upp::concepts::enum_bitmask<BitMask>);

enum class NotBitMask {
    A = 1,
    B = 2,
    C = 3,
};
static_assert(!upp::concepts::enum_bitmask<NotBitMask>);

