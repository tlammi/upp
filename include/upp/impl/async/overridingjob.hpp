/* SPDX-License-Identifier: MIT */
/* Copyright @ 2020 Toni Lammi */
#pragma once

#include <future>
#include <tuple>

#include "upp/impl/async/detail.hpp"
#include "upp/impl/async/executor.hpp"
#include "upp/impl/async/schedulable.hpp"
#include "upp/impl/traits/callable_traits.hpp"

namespace upp {
namespace async {

/**
 * \brief Job which, on rescheduling, overrides the last invocation
 *
 * If the job is scheduled and operator() is called before the job
 * is invoked, the position of the job is kept in the Executor's queue
 * but the arguments are replaced. If no pending invocation exists,
 * the object is rescheduled.
 */
template <typename Callable>
class OverridingJob : public Schedulable {
public:
		using Ret = typename traits::callable_traits<Callable>::ret_t;
		static_assert(std::is_same_v<Ret, void>);
		using Tuple = typename traits::callable_traits<Callable>::arg_tuple_t;

		OverridingJob(Executor& exec, Callable&& f) : f_{f}, exec_{exec} {}
		OverridingJob(Executor& exec, const Callable& f) : f_{f}, exec_{exec} {}

		OverridingJob(const OverridingJob&) = delete;
		OverridingJob(OverridingJob&&) = delete;

		~OverridingJob() { exec_.cancel(*this); }

		OverridingJob& operator=(const OverridingJob&) = delete;
		OverridingJob& operator=(OverridingJob&&) = delete;
		template <typename... Args>
		void operator()(Args... args) {
				std::unique_lock lk{mut_};
				args_ = std::make_tuple(std::forward<Args>(args)...);
				if (!scheduled_) { exec_.schedule(*this, 0); }
				scheduled_ = true;
		}

		void run() final {
				Tuple args;
				{
						std::unique_lock lk{mut_};
						scheduled_ = false;
						args = std::move(args_);
				}
				std::apply(detail::ApplyWrapper(f_), std::move(args));
		}

private:
		Callable f_;
		Executor& exec_;
		std::mutex mut_{};
		Tuple args_{};
		bool scheduled_{false};
};
template <typename Ret, typename... Args>
OverridingJob(Executor&, Ret(Args...)) -> OverridingJob<Ret (*)(Args...)>;

}  // namespace async
}  // namespace upp