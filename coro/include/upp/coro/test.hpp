#pragma once

#include <gtest/gtest.h>

#define CO_TEST(suite, name)                                              \
    ::upp::coro::task<void> test_fn_##suite##_##name();                   \
    TEST(suite, name) { stdexec::sync_wait(test_fn_##suite##_##name()); } \
    ::upp::coro::task<void> test_fn_##suite##_##name()

#define CO_ASSERT_IMPL(macro, ...)          \
    if ([&] {                               \
            bool co_assert_success = false; \
            [&] {                           \
                macro(__VA_ARGS__);         \
                co_assert_success = true;   \
            }();                            \
            return co_assert_success;       \
        }())                                \
        ;                                   \
    else                                    \
        co_return;

#define CO_ASSERT_TRUE(expr)  CO_ASSERT_IMPL(ASSERT_TRUE, expr)
#define CO_ASSERT_FALSE(expr) CO_ASSERT_IMPL(ASSERT_FALSE, expr)
#define CO_ASSERT_EQ(a, b)    CO_ASSERT_IMPL(ASSERT_EQ, a, b)
