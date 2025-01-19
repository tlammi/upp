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
