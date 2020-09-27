#pragma once
#include "upp/impl/async/executor.hpp"
#include "upp/impl/timer/oneshot.hpp"
#include "upp/impl/traits/callable_traits.hpp"
namespace upp {
namespace async {

/**
 * \brief Interval job, executed continuously with sleeps in between
 *
 * The job starts automatically in constructor and is rescheduled after each
 * execution.
 */
template <typename Callable, typename Rep, typename Period>
class IntervalJob : public Schedulable {
public:
		using Ret = typename traits::callable_traits<Callable>::ret_t;
		static_assert(std::is_same_v<Ret, void>);
		using Tuple = typename traits::callable_traits<Callable>::arg_tuple_t;
		static_assert(std::is_same_v<Tuple, std::tuple<>>);

		/**
		 * \brief Create and start an IntervalJob
		 *
		 * \param exec Executor used for executing the wrapped Callable
		 * \param timer Timer used for sleeping
		 * \param dur Duration slept after each invocation
		 * \param priority Job priority. Inverted
		 * \param f Callable invoked after each interval
		 */
		IntervalJob(Executor& exec,
					timer::OneShot<std::chrono::steady_clock>& timer,
					const std::chrono::duration<Rep, Period>& dur, Callable&& f,
					int priority = 0)
			: exec_{exec}, f_{f}, dur_{dur}, timer_{timer}, prio_{priority} {
				exec_.schedule(*this, prio_);
		}

		~IntervalJob() { exec_.cancel(*this); }

		void run() {
				f_();
				timer_.after(dur_, [&]() { exec_.schedule(*this, prio_); });
		}

private:
		Executor& exec_;
		Callable f_;
		std::chrono::duration<Rep, Period> dur_;
		timer::OneShot<std::chrono::steady_clock>& timer_;
		int prio_;
};
}  // namespace async
}  // namespace upp