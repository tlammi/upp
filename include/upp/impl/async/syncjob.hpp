#pragma once
#include "upp/impl/async/executor.hpp"
#include "upp/impl/async/schedulable.hpp"
#include "upp/impl/traits/callable_traits.hpp"
namespace upp {
namespace async {
template <typename Callable>
class SyncJob : public Schedulable {
public:
		using Ret = typename traits::callable_traits<Callable>::ret_t;
		SyncJob(Executor& exec, Callable&& f, int priority = 0)
			: exec_{exec}, f_{f}, prio_{priority} {}

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

		void run() {
				{
						std::unique_lock lk{mut0_};
						signaled_ = true;
				}
				cv_.notify_one();
				std::unique_lock lk{mut1_};
		}

private:
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