#pragma once

#include "upp/detail/cli/opt.hpp"
namespace upp {
namespace detail {
namespace cli {

class Cmd {
public:
		explicit Cmd(const char* name = nullptr) : name_{name} {}
		Opts& opts() { return opts_; }

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

private:
		const char* name_;
		Opts opts_{};
};
}  // namespace cli
}  // namespace detail
}  // namespace upp