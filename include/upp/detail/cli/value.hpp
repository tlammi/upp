#pragma once
#include <vector>

#include "upp/detail/cli/convert.hpp"
namespace upp {
namespace cli {
class ValueBase {
public:
		virtual void add_value(const char* str) = 0;

private:
};

template <typename T>
class Value : public ValueBase {
public:
		explicit Value(T& data) : data_{data} {}

		void add_value(const char* str) {
				if (value_count_ > 0)
						throw std::invalid_argument(
							"Argument specified multiple times");
				data_ = convert_to<T>(str);
		}

private:
		T& data_;
		size_t value_count_{0};
};

template <typename T>
class Value<std::vector<T>> : public ValueBase {
public:
		explicit Value(std::vector<T>& data) : data_{data} {}

		void add_value(const char* str) { data_.push_back(convert_to<T>(str)); }

private:
		std::vector<T>& data_;
};
}  // namespace cli
}  // namespace upp