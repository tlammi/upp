#pragma once

#include <coroutine>
#include <upp/util.hpp>

namespace upp::async {

class Scheduler;

class Ctx : public Abstract {
 public:
    virtual void push_stack(std::coroutine_handle<>) = 0;
    virtual void activate() = 0;
    virtual void deactivate() = 0;
    virtual Scheduler& scheduler() const noexcept = 0;
};

}  // namespace upp::async
