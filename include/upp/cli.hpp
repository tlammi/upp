#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "upp/cli/token.hpp"

namespace upp {
namespace cli {
class Cmd;
const char** parse(Cmd& cmd, const char** iter, const char** end);

enum class ParamCount {
		None,
		One,
		Some,
};

template <typename T>
class Value {
public:
		Value(T& data) : data_{data} {}

		void add_value(const char* str);

private:
		T& data_;
};

template <>
void Value<std::string>::add_value(const char* str) {
		data_ = str;
}

template <typename T>
ParamCount param_count();
template <>
ParamCount param_count<std::string>() {
		return ParamCount::One;
}
template <>
ParamCount param_count<char>() {
		return ParamCount::One;
}

class Opt {
		friend const char** parse(Cmd&, const char**, const char**);

public:
		Opt() {}
		explicit operator bool() const { return parsed_; }
		template <typename T>
		void store_in(T& target) {
				Value<T> val{target};
				pcount_ = ::upp::cli::param_count<T>();
				set_value_func_ =
					[val = std::move(val)](const char* str) mutable {
							val.add_value(str);
					};
		}
		ParamCount param_count() const { return pcount_; }
		void set_value(const char* ptr) { set_value_func_(ptr); }

private:
		bool parsed_{false};
		ParamCount pcount_{ParamCount::None};
		std::function<void(const char*)> set_value_func_{nullptr};
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

class Cmd {
public:
		Cmd(const char* name) {}
		Opts& opts() { return opts_; }

private:
		Opts opts_{};
};

inline const char** parse(Cmd& cmd, const char** iter, const char** end) {
		auto is_flag = [](const char* str) {
				return str[0] == '-' && str[1] != '\0';
		};
		auto is_long_flag = [](const char* str) {
				return str[1] == '-' && str[2] != '\0';
		};

		while (iter != end) {
				if (is_flag(*iter)) {
						Opt* opt;
						if (is_long_flag(*iter)) {
								opt = &cmd.opts()[*(iter) + 2];
						} else {
								opt = &cmd.opts()[(*iter)[1]];
						}
						switch (opt->param_count()) {
								case ParamCount::None: {
										opt->parsed_ = true;
										break;
								}
								case ParamCount::One: {
										if (opt->parsed_)
												throw std::runtime_error(
													"Option specified twice");
										if (iter + 1 >= end)
												throw std::runtime_error(
													"No value specfied");

										opt->parsed_ = true;
										opt->set_value(*(iter + 1));
										++iter;
										break;
								}
								case ParamCount::Some: {
										throw std::runtime_error(
											"Not implemented");
								}
						}
				} else {
						return iter;
				}
				++iter;
		}
		return end;
}
}  // namespace cli
}  // namespace upp