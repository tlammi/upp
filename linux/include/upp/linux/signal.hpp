#pragma once

#include <signal.h>

#include <initializer_list>

namespace upp::linux {

/**
 * \brief Signal listener
 *
 * This sets up signal mask so that the given signals are not delivered
 * to handlers and are instead returned by the wait() method. As, in Linux,
 * child threads inherit the parent's signal mask this object should be
 * constructed in the main thread.
 * */
class signal_listener {
    sigset_t m_sigset{};

 public:
    /**
     * \brief Construct a signal listener
     *
     * This blocks the given signals.
     * */
    explicit signal_listener(std::initializer_list<int> mask);

    signal_listener(const signal_listener&) = delete;
    signal_listener& operator=(const signal_listener&) = delete;

    signal_listener(signal_listener&&) = delete;
    signal_listener& operator=(signal_listener&&) = delete;

    ~signal_listener() = default;

    /**
     * \brief Wait for the next signal
     * */
    int wait();
};
}  // namespace upp::linux
