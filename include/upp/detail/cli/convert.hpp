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
				std::stringstream ss;
				ss << str;
				T ret;
				ss >> ret;
				if constexpr (std::is_integral_v<ret_type_t<T>>) {
						if (ret == 0 && (str[0] != '0' || str[1] != '\0'))
								throw std::logic_error(
									std::string("Invalid integer value \"") +
									str + '"');
				}
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