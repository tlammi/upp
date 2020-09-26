#pragma once

#include <chrono>
#include <condition_variable>
#include <functional>
#include <map>
#include <mutex>
#include <thread>

namespace upp {
namespace timer {

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

		template <typename Rep, typename Period, typename Callable>
		void after(const std::chrono::duration<Rep, Period>& dur,
				   Callable&& f) {
				{
						std::unique_lock lk{mut_};
						wakeups_.emplace(Clock::now() + dur, f);
				}
				cv_.notify_one();
		}

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
				auto get_next_item = [&]() {
						std::unique_lock lk{mut_};
						auto tmp = std::move(*wakeups_.begin());
						wakeups_.erase(wakeups_.begin());
						return tmp;
				};
				while (true) {
						{
								std::unique_lock lk{mut_};
								cv_.wait(lk, [&]() {
										return wakeups_.size() || !operate_;
								});
						}
						if (!operate_) break;
						auto pair = get_next_item();
						std::this_thread::sleep_until(pair.first);
						pair.second();
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