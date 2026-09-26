#include <gtest/gtest.h>

#include <upp/coro/any_sender.hpp>

using upp::coro::any_sender;
using upp::coro::infallible_any_sender;
using upp::coro::trivial_any_sender;
using upp::coro::unstoppable_any_sender;

template <class T>
constexpr auto just_1() -> T {
    return stdexec::just(1);
}

template <class T>
constexpr auto test_value() {
    auto res = stdexec::sync_wait(just_1<T>());
    ASSERT_TRUE(res);
    ASSERT_EQ(std::get<0>(*res), 1);
}

using AllSenders =
    testing::Types<any_sender<int>, trivial_any_sender<int>,
                   unstoppable_any_sender<int>, infallible_any_sender<int> >;

template <class T>
class ValueTest : public ::testing::Test {};

TYPED_TEST_SUITE(ValueTest, AllSenders);

TYPED_TEST(ValueTest, ValueReturned) {
    auto res =
        stdexec::sync_wait([]() -> TypeParam { return stdexec::just(1); }());
    ASSERT_TRUE(res);
    ASSERT_EQ(std::get<0>(*res), 1);
}

using StopSenders =
    testing::Types<any_sender<int>, infallible_any_sender<int> >;

template <class T>
class StopTest : public ::testing::Test {};

TYPED_TEST_SUITE(StopTest, StopSenders);

TYPED_TEST(StopTest, StopSent) {
    auto sndr = []() -> TypeParam { return stdexec::just_stopped(); };
    auto res = stdexec::sync_wait(sndr());
    ASSERT_FALSE(res);
}

using ErrorSenders =
    testing::Types<any_sender<int>, unstoppable_any_sender<int> >;

template <class T>
class ErrorTest : public ::testing::Test {};

TYPED_TEST_SUITE(ErrorTest, ErrorSenders);

TYPED_TEST(ErrorTest, ErrorSent) {
    auto sndr = []() -> TypeParam {
        return stdexec::just() | stdexec::then([] -> int {
                   throw std::runtime_error("error");
                   return 1;
               });
    };

    auto s = sndr() | stdexec::upon_error([](std::exception_ptr) { return 2; });
    auto res = stdexec::sync_wait(std::move(s));
    ASSERT_TRUE(res);
    ASSERT_EQ(std::get<0>(*res), 2);
}
