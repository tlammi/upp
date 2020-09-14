#pragma once
#include <string>
#include <string_view>
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
		virtual std::string_view help() const = 0;

private:
};

template <typename T>
class Value : public ValueBase {
public:
		explicit Value(T& data) : data_{data} {}

		void add_value(const char* str) {
				if (value_set_)
						throw ParsingError("Argument specified multiple times");
				data_ = converter<T>::convert(str);
				value_set_ = true;
		}

		bool full() const { return value_set_; }
		std::string_view help() const { return ""; }

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

		std::string_view help() const { return " (Multiple can be specified)"; }

private:
		std::vector<T>& data_;
		std::string help_{};
};

template <typename T>
class Value<Enum<T>> : public ValueBase {
public:
		explicit Value(Enum<T>& data) : data_{data} {
				help_ = ". Supported values: ";
				bool first = true;
				for (const auto& pair : data_) {
						if (!first) help_ += ", ";
						first = false;
						help_ += pair.first;
				}
		}

		void add_value(const char* str) {
				if (value_set_)
						throw ParsingError("Argument specified multiple times");
				data_ = converter<Enum<T>>::convert(str);
				value_set_ = true;
		}

		bool full() const { return value_set_; }

		std::string_view help() const { return help_; }

private:
		bool value_set_{false};
		Enum<T>& data_;
		std::string help_{};
};
}  // namespace cli
}  // namespace detail
}  // namespace upp