#pragma once

#include <string>
#include <vector>

namespace upp {
namespace cli {

class VectValue : public std::vector<std::string> {
 public:
    VectValue() : std::vector<std::string>() {}

    template <typename T>
    std::vector<T> as() const {
        std::vector<T> out;
        for (const auto& val : *this) {
            std::stringstream ss;
            ss << val;
            T tmp;
            ss >> tmp;
            out.push_back(tmp);
        }
        return out;
    }

 private:
};
}  // namespace cli
}  // namespace upp