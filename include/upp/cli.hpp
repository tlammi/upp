#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "upp/cli/token.hpp"

namespace upp {
namespace cli {
class Cmd;
const char** parse(Cmd& cmd, const char** iter, const char** end);

class Opt {
		friend const char** parse(Cmd&, const char**, const char**);

public:
		Opt() {}
		explicit operator bool() const { return parsed_; }

private:
		bool parsed_{false};
};

class Opts {
public:
		void create(char sflag, const std::string& lflag) {
				opts_.emplace_back();
				stoo_.try_emplace(sflag, opts_.back());
				ltoo_.try_emplace(lflag, opts_.back());
		}
		void create(const std::string& lflag) {
				opts_.emplace_back();
				ltoo_.try_emplace(lflag, opts_.back());
		}
		void create(char sflag) {
				opts_.emplace_back();
				stoo_.try_emplace(sflag, opts_.back());
		}
		bool exists(char c) { return stoo_.count(c); }
		bool exists(const std::string& str) { return ltoo_.count(str); }
		Opt& operator[](char c) { return stoo_.at(c); }
		Opt& operator[](const std::string& str) { return ltoo_.at(str); }

private:
		std::vector<Opt> opts_{};
		std::unordered_map<char, Opt&> stoo_{};
		std::unordered_map<std::string, Opt&> ltoo_{};
};

class Cmd {
public:
		Cmd(const char* name) {}
		Opts& opts() { return opts_; }

private:
		Opts opts_;
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
						if (is_long_flag(*iter)) {
								cmd.opts()[*(iter + 2)].parsed_ = true;
						} else {
								cmd.opts()[(*iter)[1]].parsed_ = true;
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