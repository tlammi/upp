/* SPDX-License-Identifier: MIT */
/* Copyright @ 2020 Toni Lammi */
#pragma once

#include <atomic>

#include "upp/impl/async/executor.hpp"
#include "upp/impl/util/defer.hpp"
namespace upp {
namespace async {

/**
 * \brief Executor not spawning any additional threads.
 *
 */
class FiberExecutor : public Executor {
public:
		FiberExecutor() {}

		FiberExecutor(const FiberExecutor&) = delete;
		FiberExecutor(FiberExecutor&&) = delete;
		~FiberExecutor() {}

		FiberExecutor& operator=(const FiberExecutor&) = delete;
		FiberExecutor& operator=(FiberExecutor&&) = delete;

		void schedule(Schedulable& sched, int priority) final {
				{
						std::unique_lock lk{mut_};
						q_.insert({priority, &sched});
				}
				cv_.notify_one();
		}
		void cancel(Schedulable& sched) final {
				{
						std::unique_lock lk{mut_};
						while (true) {
								auto iter = std::find_if(
									q_.begin(), q_.end(),
									[&](const auto& item) {
											return item.sched == &sched;
									});
								if (iter != q_.end())
										q_.erase(iter);
								else
										break;
						}
				}
				std::unique_lock lk{run_mut_};
				run_cv_.wait(lk, [&]() { return sched_ != &sched; });
		}

		/**
		 *  \brief Run executor until \ref stop() is called or an exception is
		 * thrown
		 */
		void run() {
				operate_ = true;
				while (true) {
						{
								std::unique_lock lk{mut_};
								cv_.wait(lk, [&]() {
										return q_.size() || !operate_;
								});
								if (!operate_) break;
								sched_ = next_schedulable();
						}
						run_current_schedulable();
				}
		}

		/**
		 * \brief Run Schedulables from the internal buffer until the buffer is
		 * empty
		 */
		void run_all() {
				operate_ = true;
				while (operate_) {
						{
								if (!operate_) break;
								std::unique_lock lk{mut_};
								sched_ = next_schedulable();
								if (!sched_) break;
						}
						run_current_schedulable();
				}
		}

		/**
		 * \brief Stop a running Executor
		 */
		void stop() {
				{
						std::unique_lock lk{mut_};
						operate_ = false;
				}
				cv_.notify_one();
		}

private:
		/// Get next schedulable. Caller should hold \ref mut_
		Schedulable* next_schedulable() {
				if (q_.size()) {
						std::unique_lock lk2{run_mut_};
						return q_.extract(q_.begin()).value().sched;
				}
				return nullptr;
		}

		/// Run current schedulable and clean up
		void run_current_schedulable() {
				util::Defer defer{[&]() noexcept {
						{
								std::unique_lock lk{run_mut_};
								sched_ = nullptr;
						}
						run_cv_.notify_all();
				}};
				sched_->run();
		}

		std::atomic_bool operate_{true};
		std::mutex mut_{};
		std::mutex run_mut_{};
		Schedulable* sched_{nullptr};
		std::multiset<detail::SchedMeta> q_{};	///< Internal buffer
		std::condition_variable cv_{};
		std::condition_variable run_cv_{};
};
}  // namespace async
}  // namespace upp