#pragma once

namespace upp {

class Abstract {
 public:
    constexpr Abstract() noexcept = default;
    Abstract(const Abstract&) = default;
    Abstract& operator=(const Abstract&) = default;

    Abstract(Abstract&&) = default;
    Abstract& operator=(Abstract&&) = default;

    constexpr virtual ~Abstract() = default;
};
}  // namespace upp
