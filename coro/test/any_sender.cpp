#include <gtest/gtest.h>

#include <upp/coro/any_sender.hpp>
#include <upp/coro/task.hpp>

using upp::coro::any_sender;
using upp::coro::infallible_any_sender;
using upp::coro::trivial_any_sender;
using upp::coro::unstoppable_any_sender;


//
// Tests matching all senders
//

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

TYPED_TEST(ValueTest, Awaitable){
  auto sndr = []() -> TypeParam {return stdexec::just(1); }();
  auto t = [&]() -> upp::coro::task<int> {
    co_return co_await std::move(sndr);
  };
  auto res = stdexec::sync_wait(t());
  ASSERT_TRUE(res);
    ASSERT_EQ(std::get<0>(*res), 1);
}

//
// Tests for senders sending cancellation
//

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

//
// Tests for senders sending errors
//

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
