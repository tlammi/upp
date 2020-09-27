/* SPDX-License-Identifier: MIT */
/* Copyright @ 2020 Toni Lammi */

#include "upp/impl/timer/interval.hpp"

#include <gtest/gtest.h>

#include <atomic>

using namespace upp::timer;
using namespace std::literals::chrono_literals;

TEST(IntervalTimer, Ctor) {
		Interval i{100ms, []() {}};
		Interval i2{100ms, []() {}};
		Interval{10ms, []() {}};
}

TEST(IntervalTimer, Increment) {
		std::atomic<int> i{0};

		Interval t{10ms, [&]() { ++i; }};
		Interval t1{20ms, [&]() { i += 2; }};

		std::this_thread::sleep_for(100ms);

		ASSERT_GT(i, 15);
}