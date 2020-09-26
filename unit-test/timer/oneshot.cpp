#include "upp/impl/timer/oneshot.hpp"

#include <gtest/gtest.h>

using namespace upp::timer;

using namespace std::literals::chrono_literals;

TEST(OneShotTimer, Ctor) {
		OneShot timer0{};
		OneShot timer1{};
}

TEST(OneShotTimer, After) {
		OneShot timer0{};

		int i{0};
		timer0.after(100ms, [&]() { i += 1; });
		timer0.after(50ms, [&]() { i *= 2; });

		std::this_thread::sleep_for(200ms);
		ASSERT_EQ(i, 1);
}

TEST(OneShotTimer, At) {
		OneShot timer{};

		int i = 0;
		timer.at(std::chrono::steady_clock::now() + 100ms, [&]() { i *= 2; });
		timer.at(std::chrono::steady_clock::now() + 50ms, [&]() { i += 1; });

		std::this_thread::sleep_for(200ms);
		ASSERT_EQ(i, 2);
}