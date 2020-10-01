#pragma once
#include "upp/impl/async/executor.hpp"
#include "upp/impl/async/schedulable.hpp"
#include "upp/impl/traits/callable_traits.hpp"
namespace upp {
namespace async {

/**
 * \brief Job that is executed in calling thread
 *
 * This job can be used to synchronize a callable execution with other jobs
 * sharing the Executor
 */
template <typename Callable>
class SyncJob : public Schedulable {
public:
		using Ret = typename traits::callable_traits<Callable>::ret_t;

		/**
		 * \param exec Executor used for scheduling and running
		 * \param f Callable to call
		 * \param priority Priority of the job, lower number -> higher priority.
		 * Higher priority jobs do not interrupt already running tasks, but will
		 * be inserted before the lower priority tasks in the queue.
		 */
		SyncJob(Executor& exec, Callable&& f, int priority = 0)
			: exec_{exec}, f_{f}, prio_{priority} {}

		/**
		 * \brief Call wrapped callable synchronized
		 *
		 * \param args Arguments to pass to Callable
		 * \return Ret Callable return value
		 */
		template <typename... Args>
		Ret operator()(Args... args) {
				std::unique_lock lk0{mut0_};
				std::unique_lock lk1{mut1_};

				signaled_ = false;
				exec_.schedule(*this, prio_);
				cv_.wait(lk0, [&]() { return signaled_; });
				if constexpr (std::is_same_v<Ret, void>)
						f_(std::forward<Args>(args)...);
				else
						return f_(std::forward<Args>(args)...);
		}

private:
		void run() final {
				{
						std::unique_lock lk{mut0_};
						signaled_ = true;
				}
				cv_.notify_one();
				std::unique_lock lk{mut1_};
		}
		Executor& exec_;
		Callable f_;
		int prio_;
		std::mutex mut0_{};
		std::mutex mut1_{};
		std::condition_variable cv_{};
		bool signaled_{false};
};
}  // namespace async
}  // namespace upp