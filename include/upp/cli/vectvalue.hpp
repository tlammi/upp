#pragma once

#include <vector>
#include "upp/util.hpp"

namespace upp {
namespace cli {

/**
 * \brief Stores multiple command line arguments
 *
 * Basically a vector variant of the \ref Value.
 * See \ref Value for more information
 */
class VectValue : public std::vector<std::string> {
 public:
    VectValue() : std::vector<std::string>() {}

    /**
     * \brief Convert wrapped values to std::vector of another type
     *
     * \tparam T Type to convert to. All values stored in the object have to be
     * convertible to the given type.
     *
     * \return Newly constructed vector of values constructed from the wrapped
     * data
     */
    template <typename T>
    std::vector<T> as() const {
        std::vector<T> out;
        for (const auto& val : *this) { out.push_back(util::convert<T>(val)); }
        return out;
    }
};
}  // namespace cli
}  // namespace upp