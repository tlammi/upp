#include <gtest/gtest.h>

#include <upp/template_str.hpp>

template <upp::template_str Str>
struct foo {
    static constexpr auto value = Str.view();
};

static_assert(foo<"bar">::value == "bar");
