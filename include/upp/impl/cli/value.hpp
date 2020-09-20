#pragma once
#include <string>
#include <string_view>
#include <vector>

#include "upp/impl/cli/convert.hpp"
namespace upp {
namespace cli {

class ValueBase {
public:
		virtual ~ValueBase() {}
		virtual void add_value(const char* str) = 0;
		virtual bool full() const = 0;
		virtual std::vector<std::pair<std::string_view, std::string_view>>
		restrictions() const {
				return {};
		}
		virtual bool support_multiple() const { return false; }

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
		std::vector<std::string_view> help() const { return {}; }

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

		bool support_multiple() const { return true; }

private:
		std::vector<T>& data_;
		std::string help_{};
};

template <typename T>
class Value<Enum<T>> : public ValueBase {
public:
		explicit Value(Enum<T>& data) : data_{data} {}

		void add_value(const char* str) {
				if (value_set_)
						throw ParsingError("Argument specified multiple times");
				data_ = converter<Enum<T>>::convert(str);
				value_set_ = true;
		}

		bool full() const { return value_set_; }

		std::vector<std::pair<std::string_view, std::string_view>>
		restrictions() const {
				std::vector<std::pair<std::string_view, std::string_view>> out;
				for (const auto& pair : data_) {
						out.push_back({pair.str, pair.help});
				}
				return out;
		}

private:
		bool value_set_{false};
		Enum<T>& data_;
};
}  // namespace cli
}  // namespace upp