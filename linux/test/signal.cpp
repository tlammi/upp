#include <gtest/gtest.h>

#include <thread>
#include <upp/linux/signal.hpp>

TEST(Signal, WaitInMain) {
    auto s = upp::linux::signal_listener{SIGINT};
    auto child = std::jthread([] { ::kill(::getpid(), SIGINT); });
    auto sig = s.wait();
    ASSERT_EQ(sig, SIGINT);
}

TEST(Signal, WaitInThread) {
    auto s = upp::linux::signal_listener{SIGINT};
    int sig = 0;
    auto child = std::jthread([&] { sig = s.wait(); });
    ::kill(::getpid(), SIGINT);
    child.join();
    ASSERT_EQ(sig, SIGINT);
}
