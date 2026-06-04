#include <upp/coro/task.hpp>
#include <upp/coro/test.hpp>

upp::coro::task<int> sample() { co_return 1; }

CO_TEST(Co, Simple) {
    auto res = co_await sample();
    CO_ASSERT_EQ(res, 1);
}
