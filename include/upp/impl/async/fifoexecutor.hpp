#pragma once

#include <condition_variable>
#include <mutex>
#include <set>
#include <unordered_set>

#include "upp/impl/async/executor.hpp"

namespace upp {
namespace async {
template <size_t S>
class FifoExecutor : public Executor {
public:
		FifoExecutor() {}

		~FifoExecutor() { stop(); }

		void start() {
				for (auto& w : workers_) {
						w.thread =
							std::thread([&](Worker* w) { work_func(w); }, &w);
				}
		}

		void stop() {
				{
						std::unique_lock lk{mut_};
						operate_ = false;
				}
				cv_.notify_all();
				for (auto& w : workers_)
						if (w.thread.joinable()) w.thread.join();
		}

		void schedule(Schedulable& sched, int priority) final {
				{
						std::unique_lock lk{mut_};
						queue_.insert({priority, &sched});
				}
				cv_.notify_one();
		}

		void cancel(Schedulable& sched) final {
				{
						std::unique_lock lk{mut_};
						while (true) {
								auto iter = std::find_if(
									queue_.begin(), queue_.end(),
									[&](const auto& item) {
											return item.sched == &sched;
									});
								if (iter != queue_.end())
										queue_.erase(iter);
								else
										break;
						}
				}
				for (auto& w : workers_) {
						std::unique_lock lk{w.mutex};
						w.cv.wait(lk, [&]() { return w.sched != &sched; });
				}
		}

private:
		struct Worker {
				std::thread thread{};
				std::mutex mutex{};
				std::condition_variable cv{};
				Schedulable* sched{nullptr};
		};
		void work_func(Worker* worker) {
				while (true) {
						{
								std::unique_lock lk{mut_};
								cv_.wait(lk, [&]() {
										return queue_.size() || !operate_;
								});
								if (!operate_) break;
								if (queue_.size()) {
										std::unique_lock lk2{worker->mutex};
										worker->sched =
											queue_.extract(queue_.begin())
												.value()
												.sched;
								}
						}
						worker->sched->run();
						{
								std::unique_lock lk{worker->mutex};
								worker->sched = nullptr;
						}
						worker->cv.notify_all();
				}
		}
		std::mutex mut_{};
		std::unordered_set<Schedulable*> registered_{};
		std::multiset<detail::SchedMeta> queue_{};
		std::array<Worker, S> workers_{};
		std::condition_variable cv_{};
		bool operate_{true};
};
}  // namespace async
}  // namespace upp