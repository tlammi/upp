#pragma once

#include "upp/impl/async/executor.hpp"
#include "upp/impl/util/defer.hpp"

namespace upp {
namespace async {
class FiberExecutor : public Executor {
public:
		struct SchedMeta {
				int priority;
				Schedulable* sched;
				bool operator<(const SchedMeta& rhs) const {
						return priority < rhs.priority;
				}
		};
		FiberExecutor() {}
		~FiberExecutor() {}
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

		void stop() {
				std::unique_lock lk{mut_};
				operate_ = false;
				cv_.notify_one();
		}

private:
		Schedulable* next_schedulable() {
				if (q_.size()) {
						std::unique_lock lk2{run_mut_};
						return q_.extract(q_.begin()).value().sched;
				}
				return nullptr;
		}
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
		std::multiset<SchedMeta> q_{};
		std::condition_variable cv_{};
		std::condition_variable run_cv_{};
};
}  // namespace async
}  // namespace upp