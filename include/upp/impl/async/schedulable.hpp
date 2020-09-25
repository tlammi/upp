#pragma once

namespace upp {
namespace async {

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