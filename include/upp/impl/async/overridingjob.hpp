#pragma once

#include <future>
#include <tuple>

#include "upp/impl/async/detail.hpp"
#include "upp/impl/async/schedulable.hpp"
#include "upp/impl/traits/callable_traits.hpp"

namespace upp {
namespace async {

template <typename Callable>
class OverridingJob : public Schedulable {
public:
		using Ret = typename traits::callable_traits<Callable>::ret_t;
		using Tuple = typename traits::callable_traits<Callable>::arg_tuple_t;

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
template <typename Ret, typename... Args>
OverridingJob(Executor&, Ret(Args...)) -> OverridingJob<Ret (*)(Args...)>;

}  // namespace async
}  // namespace upp