#pragma once

#include <sstream>

namespace upp {
namespace cli {
class Value {
 public:
    Value();
    Value(std::string value);

    template <typename T>
    T as() const;

    Value& operator=(const std::string& other);

 private:
    std::string _value;
};
}  // namespace cli
}  // namespace upp