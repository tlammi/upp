#pragma once

#include <type_traits>
#include <utility>

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
class Schedulable {
public:
		Schedulable() {}
		Schedulable(const Schedulable&) = delete;
		Schedulable(Schedulable&&) = delete;
		Schedulable& operator=(const Schedulable&) = delete;
		Schedulable& operator=(Schedulable&&) = delete;

		virtual void run() = 0;
};
}  // namespace async
}  // namespace upp