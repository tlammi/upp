#pragma once

#include <vector>
#include "upp/util.hpp"

namespace upp {
namespace cli {

class VectValue : public std::vector<std::string> {
 public:
    VectValue() : std::vector<std::string>() {}

    template <typename T>
    std::vector<T> as() const {
        std::vector<T> out;
        for (const auto& val : *this) { out.push_back(util::convert<T>(val)); }
        return out;
    }

 private:
};
}  // namespace cli
}  // namespace upp