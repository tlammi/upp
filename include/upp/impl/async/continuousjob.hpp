#pragma once

#include "upp/impl/async/detail.hpp"
#include "upp/impl/async/executor.hpp"
#include "upp/impl/async/schedulable.hpp"
#include "upp/impl/traits/callable_traits.hpp"

namespace upp {
namespace async {

template <typename Callable>
class ContinuousJob : public Schedulable {
public:
		using Ret = typename traits::callable_traits<Callable>::ret_t;
		static_assert(std::is_same_v<Ret, void>);
		using Tuple = typename traits::callable_traits<Callable>::arg_tuple_t;
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
ContinuousJob(Executor&, Ret(Args...)) -> ContinuousJob<Ret (*)(Args...)>;
}  // namespace async
}  // namespace upp