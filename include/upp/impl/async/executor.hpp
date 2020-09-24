#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <iostream>
#include <mutex>
#include <thread>

#include "upp/impl/async/schedulable.hpp"
#include "upp/sync/mutexwrap.hpp"
namespace upp {
namespace async {
class Executor {
public:
		virtual void schedule(Schedulable& sched, int priority) = 0;
};

template <size_t S>
class FifoExecutor : public Executor {
public:
		FifoExecutor() {
				for (size_t i = 0; i < S; ++i)
						workers_[i] = std::thread([&]() { work_func(); });
		}

		~FifoExecutor() {
				{
						std::unique_lock lk{mut_};
						operate_ = false;
				}
				cv_.notify_all();
				for (auto& w : workers_) w.join();
		}

		void schedule(Schedulable& sched, int priority) final {
				(void)priority;
				{
						std::unique_lock lk{mut_};
						queue_.push_back(&sched);
				}
				cv_.notify_one();
		}

private:
		void work_func() {
				while (true) {
						Schedulable* sched = nullptr;
						{
								std::unique_lock lk{mut_};
								cv_.wait(lk, [&]() {
										return queue_.size() || !operate_;
								});
								if (queue_.size()) {
										sched = queue_.front();
										queue_.pop_front();
								} else {
										break;
								}
						}
						sched->run();
				}
		}
		std::mutex mut_{};
		std::deque<Schedulable*> queue_{};
		std::array<std::thread, S> workers_{};
		std::condition_variable cv_{};
		std::atomic_bool operate_{true};
};
}  // namespace async
}  // namespace upp