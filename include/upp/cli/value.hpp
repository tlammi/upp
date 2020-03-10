#pragma once

#include <string>
#include "upp/util.hpp"
namespace upp {
namespace cli {
class Value {
 public:
    Value();
    Value(std::string value);

    template <typename T>
    T as() const {
        return util::convert<T>(_value);
    }

    Value& operator=(const std::string& other);

 private:
    std::string _value;
};
}  // namespace cli
}  // namespace upp