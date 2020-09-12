#pragma once
#include <vector>

#include "upp/detail/cli/convert.hpp"
namespace upp {
namespace detail {
namespace cli {

class ValueBase {
public:
		virtual ~ValueBase() {}
		virtual void add_value(const char* str) = 0;
		virtual bool full() const = 0;

private:
};

template <typename T>
class Value : public ValueBase {
public:
		explicit Value(T& data) : data_{data} {}

		void add_value(const char* str) {
				if (value_set_)
						throw std::invalid_argument(
							"Argument specified multiple times");
				data_ = converter<T>::convert(str);
				value_set_ = true;
		}

		bool full() const { return value_set_; }

private:
		T& data_;
		bool value_set_{false};
};

template <typename T>
class Value<std::vector<T>> : public ValueBase {
public:
		explicit Value(std::vector<T>& data) : data_{data} {}

		void add_value(const char* str) {
				data_.emplace_back(converter<T>::convert(str));
		}

		bool full() const { return false; }

private:
		std::vector<T>& data_;
};
}  // namespace cli
}  // namespace detail
}  // namespace upp