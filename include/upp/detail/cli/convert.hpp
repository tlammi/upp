#pragma once

#include <sstream>
#include <stdexcept>
#include <string>

#include "upp/detail/cli/enum.hpp"
namespace upp {
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

template <typename T>
struct converter {
		static ret_type_t<T> convert(const char* str) {
				if constexpr (std::is_same_v<std::string, ret_type_t<T>> ||
							  std::is_same_v<const char*, ret_type_t<T>>) {
						return str;
				} else if (std::is_floating_point_v<ret_type_t<T>>) {
						return handle_floating(str);
				} else if (std::is_integral_v<ret_type_t<T>>) {
						return handle_integer(str);
				} else {
						std::runtime_error("Unsupported conversion");
				}
		}
		static ret_type_t<T> handle_floating(const char* str) {
				bool dot{false};
				for (const char* c = str; *c != '\0'; ++c) {
						if (*c == '.') {
								if (dot)
										throw std::logic_error(
											"Invalid floating point "
											"value");
								dot = true;
						} else {
								if (!std::isdigit(*c))
										throw std::logic_error(
											"Invalid floating point "
											"value");
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
										throw std::logic_error(
											"Invalid integer");
								}
						}
						ss << std::hex << str;
				} else {
						for (const char* c = str; *c != '\0'; ++c) {
								if (!std::isdigit(*c))
										throw std::logic_error(
											"Invalid integer");
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
}  // namespace upp