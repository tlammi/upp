/* SPDX-License-Identifier: MIT */
/* Copyright @ 2020 Toni Lammi */
#pragma once

#include <mutex>

#include "upp/impl/sync/mutexguard.hpp"
#include "upp/impl/sync/mutextryguard.hpp"

namespace upp {
namespace sync {

template <typename T>
class MutexWrap {
public:
		explicit MutexWrap(T&& val) : mut_{}, val_{std::forward<T>(val)} {}
		MutexWrap() {}

		MutexGuard<T> lock() { return MutexGuard<T>(mut_, val_); }
		MutexTryGuard<T> try_lock() { return MutexTryGuard<T>(mut_, val_); }

		template <typename Callable>
		void with_lock(Callable&& c) {
				std::unique_lock lk{mut_};
				c(val_);
		}

private:
		std::mutex mut_{};
		T val_{};
};

}  // namespace sync
}  // namespace upp
