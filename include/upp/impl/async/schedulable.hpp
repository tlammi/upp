#pragma once
namespace upp {
namespace async {

class Schedulable {
public:
		virtual void run() = 0;
};
}  // namespace async
}  // namespace upp