#include <exec/async_scope.hpp>
#include <upp/coro/detach.hpp>
#include <upp/coro/test.hpp>
#include <upp/coro/yield.hpp>

template <class T>
using task = upp::coro::task<T>;

CO_TEST(Detach, Simple) {
    auto scope = exec::async_scope();
    bool called = false;
    co_await upp::coro::detach(scope, [](auto* called) -> task<void> {
        co_await upp::coro::yield();
        *called = true;
        co_return;
    }(&called));
    CO_ASSERT_FALSE(called);
    co_await scope.on_empty();
    CO_ASSERT_TRUE(called);
}
