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
 * \brief Standard job type
 *
 * Calling \ref operator() schedules the object to the given Executor's queue
 * and stores the arguments in an intenal buffer. Later when the Executor
 * calls the Job, those arguments are passed to the callable the Job has been
 * configured in ctor.
 *
 * \tparam Callable Callable type to wrap
 * \tparam ForwardExc If true, forward exceptions to std::future, else throw
 *         NOTE: Not forwarding exceptions and running e.g. FifoExecutor will
 *         cause an abort. Since there is no way to handle the exceptions.
 */
template <typename Callable, bool ForwardExc = false>
class Job : public Schedulable {
public:
		using Ret = typename traits::callable_traits<Callable>::ret_t;
		static_assert(std::is_same_v<Ret, void>);
		using Tuple = typename traits::callable_traits<Callable>::arg_tuple_t;

		Job(Executor& exec, Callable&& f, int priority = 0)
			: f_{f}, exec_{exec}, prio_{priority} {}
		Job(Executor& exec, const Callable& f, int priority = 0)
			: f_{f}, exec_{exec}, prio_{priority} {}

		Job(const Job&) = delete;
		Job(Job&&) = delete;

		~Job() { exec_.cancel(*this); }

		Job& operator=(const Job&) = delete;
		Job& operator=(Job&&) = delete;

		template <typename... Args>
		void operator()(Args... args) {
				{
						std::unique_lock lk{mut_};
						args_.emplace_back(std::forward<Args>(args)...);
				}
				exec_.schedule(*this, prio_);
		}

private:
		void run() final {
				Tuple args;
				{
						std::unique_lock lk{mut_};
						args = std::move(args_.front());
						args_.pop_front();
				}
				std::apply(detail::ApplyWrapper(f_), std::move(args));
		}
		Callable f_;
		Executor& exec_;
		int prio_;
		std::mutex mut_{};
		std::deque<Tuple> args_{};
};

template <typename Ret, typename... Args>
Job(Executor&, Ret(Args...)) -> Job<Ret (*)(Args...)>;

}  // namespace async
}  // namespace upp