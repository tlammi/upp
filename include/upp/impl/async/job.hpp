#pragma once

#include <future>
#include <tuple>

#include "upp/impl/async/executor.hpp"
#include "upp/impl/async/schedulable.hpp"

namespace upp {
namespace async {

template <typename Callable>
class Job : public Schedulable {
public:
		using Ret = typename detail::callable_traits<Callable>::ret_t;
		using Tuple = typename detail::callable_traits<Callable>::arg_tuple_t;

		Job(Executor& exec, Callable&& f) : f_{f}, exec_{exec} {}
		Job(Executor& exec, const Callable& f) : f_{f}, exec_{exec} {}

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
				exec_.schedule(*this, 0);
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
				} catch (...) { prom.set_exception(std::current_exception()); }
		}

private:
		Callable f_;
		Executor& exec_;
		std::mutex mut_{};
		std::deque<std::promise<Ret>> proms_{};
		std::deque<Tuple> args_{};
};

template <typename Callable>
class OverridingJob : public Schedulable {
public:
		using Ret = typename detail::callable_traits<Callable>::ret_t;
		using Tuple = typename detail::callable_traits<Callable>::arg_tuple_t;

		OverridingJob(Executor& exec, Callable&& f) : f_{f}, exec_{exec} {}
		OverridingJob(Executor& exec, const Callable& f) : f_{f}, exec_{exec} {}

		~OverridingJob() { exec_.cancel(*this); }
		template <typename... Args>
		std::shared_future<Ret> operator()(Args... args) {
				std::unique_lock lk{mut_};
				args_ = std::make_tuple(std::forward<Args>(args)...);
				if (!scheduled_) {
						prom_ = std::promise<Ret>();
						fut_ = prom_.get_future();
						exec_.schedule(*this, 0);
				}
				scheduled_ = true;
				return fut_;
		}

		void run() final {
				std::unique_lock lk{mut_};
				scheduled_ = false;
				try {
						if constexpr (!std::is_same_v<Ret, void>) {
								prom_.set_value(
									std::apply(detail::ApplyWrapper(f_),
											   std::move(args_)));
						} else {
								std::apply(detail::ApplyWrapper(f_),
										   std::move(args_));
								prom_.set_value();
						}
				} catch (...) { prom_.set_exception(std::current_exception()); }
		}

private:
		Callable f_;
		Executor& exec_;
		std::promise<Ret> prom_{};
		std::shared_future<Ret> fut_{};
		std::mutex mut_{};
		Tuple args_{};
		bool scheduled_{false};
};

template <typename Callable>
class ContinuousJob : public Schedulable {
public:
		using Ret = typename detail::callable_traits<Callable>::ret_t;
		static_assert(std::is_same_v<Ret, void>);
		using Tuple = typename detail::callable_traits<Callable>::arg_tuple_t;
		static_assert(std::is_same_v<Tuple, std::tuple<>>);

		ContinuousJob(Executor& exec, Callable&& f) : f_{f}, exec_{exec} {
				exec_.schedule(*this, 0);
		}
		~ContinuousJob() { exec_.cancel(*this); }

		void run() final {
				f_();
				exec_.schedule(*this, 0);
		}

private:
		Callable f_;
		Executor& exec_;
};

template <typename Ret, typename... Args>
Job(Executor&, Ret(Args...)) -> Job<Ret (*)(Args...)>;

}  // namespace async
}  // namespace upp