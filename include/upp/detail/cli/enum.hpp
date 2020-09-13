#pragma once

#include <algorithm>
#include <initializer_list>
#include <string_view>
#include <unordered_map>

#include "upp/detail/cli/exception.hpp"
namespace upp {
namespace detail {
namespace cli {

template <typename T>
struct EnumEntry {
		T val;
		const char* str;
};

template <typename T>
class Enum {
public:
		Enum(std::initializer_list<EnumEntry<T>> args) {
				for (const auto& e : args) {
						if (map_.count(e.str))
								throw ParsingError(
									"Enum string already exists");
						map_[e.str] = e.val;
				}
		}

		bool operator==(T rhs) const { return value_ == rhs; }
		bool operator!=(T rhs) const { return value_ != rhs; }

		Enum& operator=(const char* str) {
				if (!map_.count(str)) {
						throw ParsingError("Invalid enumeration");
				}
				value_ = map_.at(str);
				return *this;
		}

		Enum& operator=(T rhs) { value_ = rhs; }

		std::string_view str() {
				auto iter = std::find_if(
					map_.begin(), map_.end(),
					[&](const auto& pair) { return value_ == pair.second; });
				if (iter != map_.end()) { return iter->first; }
		}

private:
		std::unordered_map<std::string_view, T> map_{};
		T value_{};
};
}  // namespace cli
}  // namespace detail
}  // namespace upp