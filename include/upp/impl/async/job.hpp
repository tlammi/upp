#pragma once

#include <future>
#include <tuple>

#include "upp/impl/async/detail.hpp"
#include "upp/impl/async/executor.hpp"
#include "upp/impl/async/schedulable.hpp"
#include "upp/impl/traits/callable_traits.hpp"

namespace upp {
namespace async {

template <typename Callable, bool ForwardExc = false>
class Job : public Schedulable {
public:
		using Ret = typename traits::callable_traits<Callable>::ret_t;
		using Tuple = typename traits::callable_traits<Callable>::arg_tuple_t;

		Job(Executor& exec, Callable&& f, int priority = 0)
			: f_{f}, exec_{exec}, prio_{priority} {}
		Job(Executor& exec, const Callable& f, int priority = 0)
			: f_{f}, exec_{exec}, prio_{priority} {}

		~Job() { exec_.cancel(*this); }

		template <typename... Args>
		std::future<Ret> operator()(Args... args) {
				std::future<Ret> fut;
				{
						std::unique_lock lk{mut_};
						proms_.emplace_back();
						fut = proms_.back().get_future();
						args_.emplace_back(std::forward<Args>(args)...);
				}
				exec_.schedule(*this, prio_);
				return fut;
		}

		void run() final {
				std::promise<Ret> prom;
				try {
						Tuple args;
						{
								std::unique_lock lk{mut_};
								args = args_.front();
								args_.pop_front();
								prom = std::move(proms_.front());
								proms_.pop_front();
						}
						if constexpr (!std::is_same_v<Ret, void>) {
								prom.set_value(std::apply(
									detail::ApplyWrapper(f_), std::move(args)));
						} else {
								std::apply(detail::ApplyWrapper(f_),
										   std::move(args));
								prom.set_value();
						}
				} catch (...) {
						if constexpr (ForwardExc)
								prom.set_exception(std::current_exception());
						else
								throw;
				}
		}

private:
		Callable f_;
		Executor& exec_;
		int prio_;
		std::mutex mut_{};
		std::deque<std::promise<Ret>> proms_{};
		std::deque<Tuple> args_{};
};

template <typename Ret, typename... Args>
Job(Executor&, Ret(Args...)) -> Job<Ret (*)(Args...)>;

}  // namespace async
}  // namespace upp