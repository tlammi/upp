#include "upp/detail/cli/convert.hpp"

namespace upp {
namespace cli {

template <>
const char* convert_to(const char* str) {
		return str;
}

template <>
std::string convert_to(const char* str) {
		return str;
}

template <>
int8_t convert_to(const char* str) {
		if (str[1] != '\0')
				throw std::invalid_argument("Cannot convert to str");
		return str[0];
}

template <>
uint8_t convert_to(const char* str) {
		return static_cast<uint8_t>(convert_to<int8_t>(str));
}

}  // namespace cli
}  // namespace upp