#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "upp/detail/cli/convert.hpp"
#include "upp/detail/cli/opt.hpp"
#include "upp/detail/cli/value.hpp"

namespace upp {
namespace cli {

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

		auto get_opt = [&](auto iter) -> Opt* {
				Opt* opt;
				if (is_long_flag(*iter)) {
						opt = &cmd.opts()[*(iter) + 2];
				} else {
						opt = &cmd.opts()[(*iter)[1]];
				}
				return opt;
		};

		while (iter != end) {
				if (is_flag(*iter)) {
						Opt* opt = get_opt(iter);
						opt->parsed_ = true;
						if (opt->value()) {
								if (iter + 1 >= end)
										throw std::invalid_argument(
											"Too few arguments");
								opt->value()->add_value(*(iter + 1));
								++iter;
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