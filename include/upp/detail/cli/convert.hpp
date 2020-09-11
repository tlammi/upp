#pragma once

#include <stdexcept>
#include <string>

#include "upp/detail/cli/enum.hpp"
namespace upp {
namespace cli {
/*
template <typename T>
T convert_to(const char* str);
*/

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
		static ret_type_t<T> convert(const char* rhs);
};

template <>
struct converter<const char*> {
		static const char* convert(const char* str) { return str; }
};

template <>
struct converter<std::string> {
		static std::string convert(const char* str) { return str; }
};

template <>
struct converter<int8_t> {
		static int8_t convert(const char* str) {
				if (str[0] == '\0')
						throw std::logic_error(
							"Cannot convert nullstr to char");
				if (str[1] != '\0') throw std::logic_error("Too long string");
				return str[0];
		}
};

template <>
struct converter<uint8_t> {
		static uint8_t convert(const char* str) {
				if (str[0] == '\0')
						throw std::logic_error(
							"Cannot convert nullstr to char");
				if (str[1] != '\0') throw std::logic_error("Too long string");
				return str[0];
		}
};

template <typename T>
struct converter<Enum<T>> {
		static const char* convert(const char* str) { return str; }
};

}  // namespace cli
}  // namespace upp