#pragma once

#include <chrono>
#include <condition_variable>
#include <functional>
#include <map>
#include <mutex>
#include <thread>

namespace upp {
namespace timer {

/**
 * \brief Timer queueing individual events
 *
 * All time points managed by this object are approximations.
 */
template <typename Clock = std::chrono::steady_clock>
class OneShot {
public:
		OneShot() : t_{[&]() { work(); }} {}
		~OneShot() {
				{
						std::unique_lock lk{mut_};
						operate_ = false;
				}
				cv_.notify_one();
				t_.join();
		}

		/**
		 * \brief Schedule an event after given duration
		 *
		 * \param dur Duration from this time to sleep for
		 * \param f Callable to invoke after duration
		 */
		template <typename Rep, typename Period, typename Callable>
		void after(const std::chrono::duration<Rep, Period>& dur,
				   Callable&& f) {
				{
						std::unique_lock lk{mut_};
						wakeups_.emplace(Clock::now() + dur, f);
				}
				cv_.notify_one();
		}

		/**
		 * \brief Schedule an event at given time point
		 *
		 * \param time Time point to wake up at
		 * \param f Callable void() to invoke after the time point
		 */
		template <typename Clock2, typename Duration, typename Callable>
		void at(const std::chrono::time_point<Clock2, Duration>& time,
				Callable&& f) {
				{
						std::unique_lock lk{mut_};
						wakeups_.emplace(time, f);
				}
				cv_.notify_one();
		}

private:
		void work() {
				while (true) {
						{
								std::unique_lock lk{mut_};
								cv_.wait(lk, [&]() {
										return wakeups_.size() || !operate_;
								});
						}
						if (!operate_) break;
						while (true) {
								std::unique_lock lk{mut_};
								if (cv_.wait_until(lk,
												   wakeups_.begin()->first) ==
									std::cv_status::timeout) {
										wakeups_.begin()->second();
										wakeups_.erase(wakeups_.begin());
										break;
								}
								if (!operate_) break;
						}
				}
		}
		std::thread t_;
		std::mutex mut_{};
		std::condition_variable cv_{};
		std::multimap<typename Clock::time_point, std::function<void()>>
			wakeups_{};
		bool operate_{true};
};
}  // namespace timer
}  // namespace upp