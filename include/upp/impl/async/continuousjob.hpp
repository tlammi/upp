/* SPDX-License-Identifier: MIT */
/* Copyright @ 2020 Toni Lammi */
#pragma once

#include "upp/impl/async/detail.hpp"
#include "upp/impl/async/executor.hpp"
#include "upp/impl/async/schedulable.hpp"
#include "upp/impl/traits/callable_traits.hpp"

namespace upp {
namespace async {

/**
 * \brief Job for scheduling a job continuously
 *
 * The wrapped callable will be rescheduled after each invocation.
 * NOTE: As this job type does not return futures, exceptions are not caught
 * which will cause an abort if e.g. FifoExecutor is used.
 * \tparam Callable Wrapped callable
 */
template <typename Callable>
class ContinuousJob : public Schedulable {
public:
		using Ret = typename traits::callable_traits<Callable>::ret_t;
		static_assert(std::is_same_v<Ret, void>);
		using Tuple = typename traits::callable_traits<Callable>::arg_tuple_t;
		static_assert(std::is_same_v<Tuple, std::tuple<>>);

		ContinuousJob(Executor& exec, Callable&& f, int priority = 0)
			: f_{f}, exec_{exec}, prio_{priority} {
				exec_.schedule(*this, 0);
		}
		~ContinuousJob() { exec_.cancel(*this); }

private:
		void run() final {
				f_();
				exec_.schedule(*this, prio_);
		}
		Callable f_;
		Executor& exec_;
		int prio_;
};
template <typename Ret, typename... Args>
ContinuousJob(Executor&, Ret(Args...)) -> ContinuousJob<Ret (*)(Args...)>;
}  // namespace async
}  // namespace upp