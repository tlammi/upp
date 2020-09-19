#pragma once

#include <string_view>

#include "upp/detail/cli/exception.hpp"
#include "upp/detail/cli/opt.hpp"
#include "upp/detail/cli/posargs.hpp"
namespace upp {
namespace cli {

class Cmd {
public:
		Cmd() {}

		Opts& opts() { return opts_; }
		const Opts& opts() const { return opts_; }
		PosArgs& pos_args() { return pos_args_; }
		const PosArgs& pos_args() const { return pos_args_; }

		template <typename Iter>
		Iter parse(Iter iter, Iter end) {
				auto is_flag = [](const char* str) {
						return str[0] == '-' && str[1] != '\0';
				};
				auto is_long_flag = [](const char* str) {
						return str[1] == '-' && str[2] != '\0';
				};

				auto get_opt = [&](auto iter) -> Opt* {
						Opt* opt;
						if (is_long_flag(*iter)) {
								opt = &opts()[*(iter) + 2];
						} else {
								opt = &opts()[(*iter)[1]];
						}
						return opt;
				};
				while (iter != end && is_flag(*iter)) {
						Opt* opt = get_opt(iter);
						opt->parsed_ = true;
						if (opt->value_) {
								if (iter + 1 >= end)
										throw ParsingError("Too few arguments");
								opt->value_->add_value(*(iter + 1));
								++iter;
						}
						if (opt->cb_) opt->cb_();
						++iter;
				}
				if (pos_args_.value_) {
						while (iter != end) {
								if (pos_args_.value_->full()) return iter;
								pos_args_.value_->add_value(*(iter));
								++iter;
						}
				}
				return iter;
		}

private:
		Opts opts_{};
		PosArgs pos_args_{};
};
}  // namespace cli
}  // namespace upp