#pragma once

#include <stdexcept>
#include <string_view>
namespace upp {
namespace strutil {
namespace detail {
constexpr std::string_view WHITESPACES = " \n\r\t\v\f";
}

constexpr bool isspace(char c) {
		for (auto ws : detail::WHITESPACES)
				if (ws == c) return true;
		return false;
}
constexpr bool isdigit(char c) { return c >= '0' && c <= '9'; }

constexpr char todigit(char c) {
		if (!isdigit(c)) throw std::runtime_error("Not a digit");
		return c - '0';
}

constexpr std::string_view strip(std::string_view str) {
		size_t f = str.find_first_not_of(detail::WHITESPACES);
		if (f == std::string_view::npos) return "";
		size_t l = str.find_last_not_of(detail::WHITESPACES);
		return str.substr(f, l - f + 1);
}

namespace detail {
template <typename T>
constexpr T tonum_before_dot(std::string_view& str) {
		T out{0};
		while (str.size() && str[0] != '.') {
				char d = todigit(str[0]);
				out *= 10;
				out += d;
				str.remove_prefix(1);
		}
		return out;
}

template <typename T>
constexpr T tonum_after_dot(std::string_view& str) {
		if constexpr (std::is_integral_v<T>) {
				while (str.size()) {
						if (!isdigit(str[0]))
								throw std::runtime_error("Not a digit");
						str.remove_prefix(1);
				}
				return 0;
		}
		T out{0};
		double base = 10;
		while (str.size()) {
				if (!isdigit(str[0])) throw std::runtime_error("Not a digit");
				T d = str[0] - '0';
				out += d / base;
				base *= 10;
				str.remove_prefix(1);
		}
		return out;
}
}  // namespace detail

template <typename T>
constexpr T tonum(std::string_view str) {
		str = strip(str);
		T out = detail::tonum_before_dot<T>(str);
		if (str.size() && str[0] == '.') str.remove_prefix(1);
		out += detail::tonum_after_dot<T>(str);
		return out;
}

}  // namespace strutil
}  // namespace upp