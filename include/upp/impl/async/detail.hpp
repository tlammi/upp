/* SPDX-License-Identifier: MIT */
/* Copyright @ 2020 Toni Lammi */
#pragma once

#include "upp/impl/async/schedulable.hpp"
#include "upp/impl/traits/callable_traits.hpp"

namespace upp {
namespace async {
namespace detail {

/**
 * \brief Helper to allow to call std::apply without move
 */
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
/**
 * \brief Container struct for internal multimap
 */
struct SchedMeta {
		int priority;  ///< Used for sorting only
		Schedulable* sched;
		bool operator<(const SchedMeta& rhs) const {
				return priority < rhs.priority;
		}
};
}  // namespace detail
}  // namespace async
}  // namespace upp