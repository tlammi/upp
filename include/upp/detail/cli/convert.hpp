#pragma once

#include <sstream>
#include <stdexcept>
#include <string>

#include "upp/detail/cli/enum.hpp"
#include "upp/detail/cli/exception.hpp"
namespace upp {
namespace detail {
namespace cli {

template <typename T>
struct ret_type {
		using type = T;
};

template <typename T>
struct ret_type<Enum<T>> {
		using type = const char*;
};

template <typename T>
using ret_type_t = typename ret_type<T>::type;

template <auto A, typename...>
auto asserting_value = A;

template <typename T>
struct converter {
		static ret_type_t<T> convert(const char* str) {
				if constexpr (std::is_same_v<std::string, ret_type_t<T>> ||
							  std::is_same_v<const char*, ret_type_t<T>>) {
						return str;
				} else if constexpr (std::is_floating_point_v<ret_type_t<T>>) {
						return handle_floating(str);
				} else if constexpr (std::is_integral_v<ret_type_t<T>>) {
						return handle_integer(str);
				} else {
						static_assert(asserting_value<false, T>,
									  "Unsupported conversion");
				}
		}
		static ret_type_t<T> handle_floating(const char* str) {
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
				ret_type_t<T> ret;
				ss >> ret;
				return ret;
		}

		static ret_type_t<T> handle_integer(const char* str) {
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
				ret_type_t<T> ret;
				ss >> ret;
				return ret;
		}

		static void run_integer_checks(const ret_type_t<T>& val) {}
};

template <typename T>
struct converter<Enum<T>> {
		static const char* convert(const char* str) { return str; }
};

}  // namespace cli
}  // namespace detail
}  // namespace upp