#pragma once

#include <initializer_list>
#include <unordered_map>

namespace upp {
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
								throw std::runtime_error(
									"Enum string already exists");
						map_[e.str] = e.val;
				}
		}

		bool operator==(T rhs) const { return value_ == rhs; }
		bool operator!=(T rhs) const { return value_ != rhs; }

		Enum& operator=(const char* str) {
				value_ = map_.at(str);
				return *this;
		}
		Enum& operator=(T rhs) { value_ = rhs; }

private:
		std::unordered_map<const char*, T> map_{};
		T value_{};
};
}  // namespace cli
}  // namespace upp