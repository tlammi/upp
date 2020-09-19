#pragma once

#include <string_view>

#include "upp/detail/cli/exception.hpp"
#include "upp/detail/cli/opt.hpp"
#include "upp/detail/cli/posargs.hpp"
namespace upp {
namespace cli {

/**
 * @brief Main part of the CLI parser
 *
 * Represents an option on CLI, i.e. <cmd> in <cmd> [options] [arguments].
 *
 */
class Cmd {
public:
		Cmd() {}

		/**
		 * @brief Access Opt container
		 *
		 * @return Opts& Reference to options part of the command:
		 */
		Opts& opts() { return opts_; }

		/**
		 * @brief Const variant for opts()
		 *
		 */
		const Opts& opts() const { return opts_; }

		/**
		 * @brief Access positional arguments collection
		 *
		 * @return PosArgs& Reference to positional arguments handler of the
		 * object
		 */
		PosArgs& pos_args() { return pos_args_; }

		/**
		 * @brief Const variant for pos_args()
		 *
		 */
		const PosArgs& pos_args() const { return pos_args_; }

		/**
		 * @brief Parse command line arguments to populate the command
		 *
		 * @tparam Iter Iterator, mainly a const char* const *
		 * @param iter Pointer to first arugment, normally argv+1
		 * @param end Pointer to end of the argumetns, normally argv+argc
		 * @return Iter First iterator NOT handled. That can then be used to
		 * parse sub commands
		 */
		template <typename Iter>
		Iter parse(Iter iter, Iter end) {
				auto is_short_flag = [](const char* str) {
						return str[0] == '-' && str[1] != '\0' &&
							   str[1] != '-' && str[2] == '\0';
				};
				auto is_long_flag = [](const char* str) {
						return str[0] == '-' && str[1] == '-' &&
							   str[2] != '-' && str[2] != '\0';
				};
				auto is_flag = [&](const char* str) { return str[0] == '-'; };

				auto get_opt = [&](auto iter) -> Opt* {
						Opt* opt;
						if (is_long_flag(*iter)) {
								opt = &opts()[*(iter) + 2];
						} else if (is_short_flag(*iter)) {
								opt = &opts()[(*iter)[1]];
						} else {
								std::stringstream ss;
								ss << "Invalid flag: " << *iter;
								throw ParsingError(ss.str());
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