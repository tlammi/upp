#pragma once

#include "upp/detail/cli/cmd.hpp"

namespace upp {
namespace detail {
namespace cli {
template <typename Iter>
Iter parse(Cmd& cmd, Iter iter, Iter end) {
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
						if (opt->value_) {
								if (iter + 1 >= end)
										throw std::invalid_argument(
											"Too few arguments");
								opt->value_->add_value(*(iter + 1));
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
}  // namespace detail
}  // namespace upp