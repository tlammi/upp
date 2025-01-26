#pragma once

namespace upp {

class Abstract {
 public:
    Abstract(const Abstract&) = delete;
    Abstract& operator=(const Abstract&) = delete;

    Abstract(Abstract&&) = delete;
    Abstract& operator=(Abstract&&) = delete;

    constexpr virtual ~Abstract() = default;
};
}  // namespace upp
