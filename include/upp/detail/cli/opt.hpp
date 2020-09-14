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

		std::vector<std::string_view> value_restrictions() const {
				if (value_) return value_->value_restrictions();
				return {};
		}

		bool support_multiple_values() const {
				if (value_) return value_->support_multiple_values();
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
		Opt& create(char sflag, const std::string& lflag) {
				opts_.emplace_back();
				stoo_[sflag] = opts_.size() - 1;
				ltoo_[lflag] = opts_.size() - 1;
				return opts_.back();
		}
		Opt& create(const std::string& lflag) {
				opts_.emplace_back();
				ltoo_[lflag] = opts_.size() - 1;
				return opts_.back();
		}
		Opt& create(char sflag) {
				opts_.emplace_back();
				stoo_[sflag] = opts_.size() - 1;
				return opts_.back();
		}
		bool exists(char c) { return stoo_.count(c); }
		bool exists(const std::string& str) { return ltoo_.count(str); }
		Opt& operator[](char c) { return opts_.at(stoo_.at(c)); }
		Opt& operator[](const std::string& str) {
				return opts_.at(ltoo_.at(str));
		}

		class ConstOptIter {
		public:
				struct OptSet {
						OptSet(char sflag, const char* lflag, const Opt& opt)
							: short_flag{sflag}, long_flag{lflag}, opt{opt} {}
						char short_flag;
						const char* long_flag;
						const Opt& opt;
				};

				ConstOptIter(const Opts* opts, size_t index)
					: opts_{opts}, index_{index} {}

				bool operator==(const ConstOptIter& rhs) const {
						return opts_ == rhs.opts_ && index_ == rhs.index_;
				}

				bool operator!=(const ConstOptIter& rhs) const {
						return !(*this == rhs);
				}

				ConstOptIter& operator++() {
						++index_;
						return *this;
				}

				const OptSet& operator*() const {
						auto short_iter = std::find_if(
							opts_->stoo_.begin(), opts_->stoo_.end(),
							[&](const auto& pair) {
									return pair.second == index_;
							});
						auto long_iter = std::find_if(
							opts_->ltoo_.begin(), opts_->ltoo_.end(),
							[&](const auto& pair) {
									return pair.second == index_;
							});

						char c = short_iter == opts_->stoo_.end()
									 ? '\0'
									 : short_iter->first;
						const char* str = long_iter == opts_->ltoo_.end()
											  ? ""
											  : long_iter->first.c_str();

						opt_set_.emplace(c, str, opts_->opts_.at(index_));
						return *opt_set_;
				}

		private:
				const Opts* opts_;
				size_t index_;
				mutable std::optional<OptSet> opt_set_{};
		};

		ConstOptIter begin() const { return ConstOptIter(this, 0); }
		ConstOptIter end() const { return ConstOptIter(this, opts_.size()); }

		size_t size() const { return opts_.size(); }

private:
		std::vector<Opt> opts_{};
		std::map<char, size_t> stoo_{};
		std::map<std::string, size_t> ltoo_{};
};
}  // namespace cli
}  // namespace detail
}  // namespace upp