#pragma once

#include "upp/impl/traits/callable_traits.hpp"

namespace upp {
namespace async {
namespace detail {

template <typename Callable>
struct ApplyWrapper {
		using Ret = typename traits::callable_traits<Callable>::ret_t;
		explicit ApplyWrapper(Callable& f) : f_{f} {}

		template <typename... Args>
		Ret operator()(Args... args) {
				return f_(std::forward<Args>(args)...);
		}

		Callable& f_;
};
}  // namespace detail
}  // namespace async
}  // namespace upp