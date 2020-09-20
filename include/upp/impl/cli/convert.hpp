#pragma once

/**
 * @file convert.hpp Conversion utils for string -> type conversion
 * @author tlammi (tlammi@github.com)
 * @brief
 * @version 0.1
 * @date 2020-09-20
 *
 * @copyright Copyright (c) 2020
 *
 */
#include <sstream>
#include <stdexcept>
#include <string>

#include "upp/impl/cli/enum.hpp"
#include "upp/impl/cli/exception.hpp"

namespace upp {
namespace cli {

template <auto A, typename...>
auto asserting_value = A;

template <typename T>
struct converter {
		static T convert(const char* str) {
				if constexpr (std::is_same_v<std::string, T> ||
							  std::is_same_v<const char*, T>) {
						return str;
				} else if constexpr (std::is_floating_point_v<T>) {
						return handle_floating(str);
				} else if constexpr (std::is_integral_v<T>) {
						return handle_integer(str);
				} else {
						static_assert(asserting_value<false, T>,
									  "Unsupported conversion");
				}
		}

private:
		static T handle_floating(const char* str) {
				bool dot{false};
				for (const char* c = str; *c != '\0'; ++c) {
						if (*c == '.') {
								if (dot)
										throw ParsingError(
											"Invalid floating point");
								dot = true;
						} else {
								if (!std::isdigit(*c))
										throw ParsingError(
											"Invalid floating point");
						}
				}
				std::stringstream ss;
				ss << str;
				T ret;
				ss >> ret;
				return ret;
		}

		static T handle_integer(const char* str) {
				std::stringstream ss;
				if (str[0] == '0' && str[1] == 'x') {
						for (const char* c = &str[2]; *c != '\0'; ++c) {
								if (*c >= '0' && *c <= '9') {
								} else if (*c >= 'A' && *c <= 'F') {
								} else if (*c >= 'a' && *c <= 'f') {
								} else {
										throw ParsingError("Invalid integer");
								}
						}
						ss << std::hex << str;
				} else {
						for (const char* c = str; *c != '\0'; ++c) {
								if (!std::isdigit(*c))
										throw ParsingError("Invalid integer");
						}
						ss << str;
				}
				T ret;
				ss >> ret;
				return ret;
		}

		static void run_integer_checks(const T& val) {}
};

template <typename T>
struct converter<Enum<T>> {
		static const char* convert(const char* str) { return str; }
};

}  // namespace cli
}  // namespace upp