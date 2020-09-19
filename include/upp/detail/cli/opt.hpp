#pragma once

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string_view>

#include "upp/detail/cli/value.hpp"

namespace upp {
namespace detail {
namespace cli {
class Cmd;

class Opt {
		friend class Cmd;

public:
		Opt() {}
		explicit operator bool() const { return parsed_; }
		template <typename T>
		Opt& store_in(T& target) {
				value_ = std::make_unique<Value<T>>(target);
				return *this;
		}
		template <typename Func>
		Opt& callback(Func&& func) {
				cb_ = func;
				return *this;
		}

		Opt& set_help(std::string_view help_str) {
				help_ = help_str;
				return *this;
		}

		std::vector<std::pair<std::string_view, std::string_view>>
		value_restrictions() const {
				if (value_) return value_->value_restrictions();
				return {};
		}

		bool support_multiple() const {
				if (value_) return value_->support_multiple();
				return false;
		}

		std::string_view help() const { return help_; }

private:
		bool parsed_{false};
		std::unique_ptr<ValueBase> value_{nullptr};
		std::string help_{};
		std::function<void()> cb_{nullptr};
};

class Opts {
public:
		struct OptMeta {
				char short_flag;
				std::string long_flag;
		};

		struct OptMetaCmp {
				bool operator()(const OptMeta& lhs, const OptMeta& rhs) const {
						return lhs.long_flag < rhs.long_flag;
				}
		};

		using Map = std::map<OptMeta, Opt, OptMetaCmp>;

		Opt& create(char sflag, const std::string& lflag) {
				if (key_exists(sflag, lflag))
						throw std::invalid_argument("Option exists");
				return opts_[{sflag, lflag}] = {};
		}

		Opt& create(const std::string& lflag) {
				if (key_exists('\0', lflag))
						throw std::invalid_argument("Option exists");
				return opts_[{'\0', lflag}] = {};
		}
		Opt& create(char sflag) {
				if (key_exists(sflag, ""))
						throw std::invalid_argument("Option exists");
				return opts_[{sflag, ""}] = {};
		}
		bool exists(char c) { return key_exists(c, ""); }
		bool exists(const std::string& str) { return key_exists('\0', str); }
		Opt& operator[](char c) {
				auto iter = std::find_if(
					opts_.begin(), opts_.end(), [&](const auto& pair) {
							return pair.first.short_flag == c;
					});
				if (iter != opts_.end()) return iter->second;
				throw std::invalid_argument("Option does not exist");
		}
		Opt& operator[](const std::string& str) {
				auto iter = std::find_if(
					opts_.begin(), opts_.end(), [&](const auto& pair) {
							return pair.first.long_flag == str;
					});
				if (iter != opts_.end()) return iter->second;
				throw std::invalid_argument("Option does not exist");
		}

		Map::const_iterator begin() const { return opts_.begin(); }
		Map::const_iterator end() const { return opts_.end(); }

		size_t size() const { return opts_.size(); }

private:
		bool key_exists(char sflag, std::string_view lflag) const {
				return std::find_if(
						   opts_.begin(), opts_.end(), [&](const auto& pair) {
								   return (sflag != '\0' &&
										   pair.first.short_flag == sflag) ||
										  (lflag != "" &&
										   pair.first.long_flag == lflag);
						   }) != opts_.end();
		}
		Map opts_{};
};
}  // namespace cli
}  // namespace detail
}  // namespace upp