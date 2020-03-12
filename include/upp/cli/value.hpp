#pragma once

#include <string>
#include "upp/util.hpp"
namespace upp {
namespace cli {

/**
 * \brief Class for storing values parsed from CLI
 *
 * "Value" is a term used to denote value specified after the flag in CLI, e.g.
 * ./program --option **value**. The class wraps the string representation of
 * the parsed data and can be used to convert the data to different primitive
 * types.
 *
 */
class Value {
 public:
    Value();

    /**
     * \brief Construct a Value wrapping the given string
     *
     * \param value Value to wrap
     */
    explicit Value(const std::string& value);

    /**
     * \brief Convert the wrapped value to a primitive type
     *
     * \tparam T Type to convert to
     */
    template <typename T>
    T as() const {
        return util::convert<T>(_value);
    }

    /**
     * \brief Assign operator
     *
     * Overloaded so values can be easily updated into the container containing
     * the object. E.g. std::map:  `mymap["flag"] = "value";` where mymap is
     * instance of `std::map<std::string,Value>`
     */
    Value& operator=(const std::string& other);

 private:
    std::string _value;
};
}  // namespace cli
}  // namespace upp