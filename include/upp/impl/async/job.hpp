#pragma once

#include <future>
#include <tuple>

#include "upp/impl/async/executor.hpp"
#include "upp/impl/async/schedulable.hpp"

namespace upp {
namespace async {
namespace detail {

template <typename T>
struct callable_traits : public callable_traits<decltype(&T::operator())> {};

template <typename Ret, typename... Args>
struct callable_traits<Ret (*)(Args...)> {
		using ret_t = Ret;
		using arg_tuple_t = std::tuple<Args...>;
};

template <typename Ret, typename Class, typename... Args>
struct callable_traits<Ret (Class::*)(Args...) const> {
		using ret_t = Ret;
		using arg_tuple_t = std::tuple<Args...>;
};
template <typename Ret, typename Class, typename... Args>
struct callable_traits<Ret (Class::*)(Args...)> {
		using ret_t = Ret;
		using arg_tuple_t = std::tuple<Args...>;
};

template <typename Callable>
struct ApplyWrapper {
		using Ret = typename callable_traits<Callable>::ret_t;
		explicit ApplyWrapper(Callable& f) : f_{f} {}

		template <typename... Args>
		Ret operator()(Args... args) {
				return f_(std::forward<Args>(args)...);
		}

		Callable& f_;
};
}  // namespace detail

template <typename Callable>
class Job : public Schedulable {
public:
		using Ret = typename detail::callable_traits<Callable>::ret_t;
		using Tuple = typename detail::callable_traits<Callable>::arg_tuple_t;

		Job(Executor* exec, Callable&& f) : f_{f}, exec_{exec} {}
		Job(Executor* exec, const Callable& f) : f_{f}, exec_{exec} {}

		template <typename... Args>
		std::future<Ret> operator()(Args... args) {
				prom_ = std::promise<Ret>();
				auto fut = prom_.get_future();
				args_ = std::make_tuple(std::forward<Args>(args)...);
				if (exec_) exec_->schedule(*this, 0);
				return fut;
		}

		void run() final {
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
		Executor* exec_;
		std::promise<Ret> prom_{};
		Tuple args_{};
};

template <typename Ret, typename... Args>
Job(Executor*, Ret(Args...)) -> Job<Ret (*)(Args...)>;

}  // namespace async
}  // namespace upp