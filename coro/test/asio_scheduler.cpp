#include <gtest/gtest.h>

#include <upp/coro/asio.hpp>
#include <upp/coro/util.hpp>
#include <upp/unused.hpp>

using upp::coro::asio_scheduler;

TEST(Init, Simple) {
    auto ctx = boost::asio::io_context();
    auto sched = asio_scheduler(ctx);
    upp::unused(sched);
}

TEST(Schedule, NoOp) {
    auto ctx = boost::asio::io_context();

    auto worker = std::jthread([&] { ctx.run(); });

    auto sched = asio_scheduler(ctx);
    upp::coro::run(sched, [] {});
}

TEST(Schedule, CallCount) {
    auto ctx = boost::asio::io_context();
    auto worker = std::jthread([&] { ctx.run(); });

    auto sched = asio_scheduler(ctx);

    std::size_t call_count = 0;

    upp::coro::run(sched, [&] { ++call_count; });

    ASSERT_EQ(call_count, 1);
}

TEST(Schedule, Return) {
    auto ctx = boost::asio::io_context();
    auto worker = std::jthread([&] { ctx.run(); });

    auto sched = asio_scheduler(ctx);

    auto res = upp::coro::run(sched, [&] { return 42; });
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, 42);
}

TEST(Schedule, Coroutine){
    auto ctx = boost::asio::io_context();
    auto worker = std::jthread([&] { ctx.run(); });

    auto sched = asio_scheduler(ctx);

    auto res = upp::coro::run(sched, [] -> stdexec::task<int> { co_return 42; }());
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, 42);
}
