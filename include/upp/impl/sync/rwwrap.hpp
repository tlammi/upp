#pragma once

#include <shared_mutex>

namespace upp {
namespace sync {
template <typename T>
class RwWrap;
template <typename T>
class ReadGuard {
		friend RwWrap<T>;

public:
		ReadGuard(const ReadGuard&) = delete;
		ReadGuard(ReadGuard&&) = delete;
		ReadGuard& operator=(const ReadGuard&) = delete;
		ReadGuard& operator=(ReadGuard&&) = delete;

		const T& operator*() const { return t_; }
		const T* operator->() const { return &t_; }

private:
		ReadGuard(std::shared_mutex& mut, const T& t) : t_{t}, lk_{mut} {}
		const T& t_;
		std::shared_lock<std::shared_mutex> lk_;
};

template <typename T>
class WriteGuard {
		friend RwWrap<T>;

public:
		WriteGuard(const WriteGuard&) = delete;
		WriteGuard(WriteGuard&&) = delete;

		WriteGuard& operator=(const WriteGuard&) = delete;
		WriteGuard& operator=(WriteGuard&&) = delete;
		T& operator*() { return t_; }
		T* operator->() { return &t_; }

private:
		WriteGuard(std::shared_mutex& mut, T& t) : t_{t}, lk_{mut} {}
		T& t_;
		std::unique_lock<std::shared_mutex> lk_;
};
template <typename T>
class RwWrap {
public:
		RwWrap() : t_{} {}
		RwWrap(T&& t) : t_{t} {}

		ReadGuard<T> read_lock() const { return ReadGuard(mut_, t_); }
		WriteGuard<T> write_lock() { return WriteGuard(mut_, t_); }

		template <typename Callable>
		void with_read_lock(Callable&& c) const {
				std::shared_lock lk{mut_};
				c(t_);
		}
		template <typename Callable>
		void with_write_lock(Callable&& c) {
				std::unique_lock lk{mut_};
				c(t_);
		}

private:
		T t_;
		mutable std::shared_mutex mut_{};
};
}  // namespace sync
}  // namespace upp