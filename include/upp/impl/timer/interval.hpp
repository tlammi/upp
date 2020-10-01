/* SPDX-License-Identifier: MIT */
/* Copyright @ 2020 Toni Lammi */
#pragma once

#include <chrono>
#include <condition_variable>
#include <thread>

namespace upp {
namespace timer {

/**
 * \brief Interval timer
 *
 * Firing the callable with given interval slept after invocation and before
 * next one
 * 
 * \tparam Callable Callable to call
 * \tparam Rep For duration
 * \tparam Period for duration
 */
template <typename Callable, typename Rep, typename Period>
class Interval {
public:
		Interval(const std::chrono::duration<Rep, Period>& interval,
				 Callable&& callable)
			: f_{callable}, interval_{interval}, t_{[&]() { work(); }} {}

		~Interval() {
				{
						std::unique_lock lk{mut_};
						operate_ = false;
				}
				cv_.notify_one();
				t_.join();
		}

private:
		void work() {
				while (true) {
						std::unique_lock lk{mut_};
						if (cv_.wait_for(lk, interval_,
										 [&]() { return !operate_; })) {
								break;
						}
						f_();
				}
		}
		Callable f_;
		const std::chrono::duration<Rep, Period> interval_;
		std::mutex mut_{};
		std::condition_variable cv_{};
		std::thread t_;
		bool operate_{true};
};
}  // namespace timer
}  // namespace upp