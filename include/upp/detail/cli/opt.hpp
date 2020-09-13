#pragma once

#include <functional>
#include <memory>
#include <unordered_map>

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
		}

private:
		bool parsed_{false};
		std::unique_ptr<ValueBase> value_{nullptr};
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

private:
		std::vector<Opt> opts_{};
		std::unordered_map<char, size_t> stoo_{};
		std::unordered_map<std::string, size_t> ltoo_{};
};
}  // namespace cli
}  // namespace detail
}  // namespace upp