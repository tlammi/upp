#include "upp/detail/cli/helpgen.hpp"

namespace upp {
namespace cli {

namespace {
struct PrintPair {
		int w;
		std::string_view str;
};
void print_columnized(std::stringstream& ss,
					  const std::vector<PrintPair>& vect) {
		for (const auto& [w, str] : vect) {
				ss << std::setw(w) << std::left << str;
		}
}

std::deque<std::string> split_to_lines(std::string_view str, int columns) {
		if (columns <= 0) return {std::string(str)};
		int i = 0;
		std::deque<std::string> out{};
		for (const auto& c : str) {
				if (!i) {
						out.emplace_back();
						i = 0;
				}
				out.back().push_back(c);
				i = (i + 1) % columns;
		}
		return out;
}

void print_brief(std::stringstream& ss, std::string_view brief) {
		if (brief == "") return;
		ss << brief << "\n\n";
		return;
}

void print_usage(std::stringstream& ss, std::string_view usage) {
		if (usage == "") return;
		ss << usage << "\n\n";
		return;
}

void print_opt(std::stringstream& ss,
			   const std::pair<const Opts::OptMeta, Opt>& pair,
			   int max_lflag_len, int line_width) {
		const auto& [meta, opt] = pair;
		const auto& [sflag, lflag] = meta;
		std::string sflag_str{};
		std::string lflag_str{};
		const int SFLAG_COL = 3;
		const int LFLAG_COL = max_lflag_len + 4;
		const int SUPPORTED_VALUE_INDENTATION = max_lflag_len + 7;
		const int SUPPORTED_VALUE_COL =
			line_width - SUPPORTED_VALUE_INDENTATION;
		const int HELP_COL = line_width - SFLAG_COL - LFLAG_COL;

		if (sflag != '\0') {
				sflag_str.push_back('-');
				sflag_str.push_back(sflag);
		} else {
				sflag_str = "  ";
		}
		if (lflag != "") {
				lflag_str = "--";
				lflag_str += lflag;
		} else {
				lflag_str = std::string(max_lflag_len + 2, ' ');
		}
		auto helps =
			split_to_lines(opt.help(), line_width - LFLAG_COL - SFLAG_COL);
		if (helps.size())
				print_columnized(ss, {
										 {SFLAG_COL, sflag_str},
										 {LFLAG_COL, lflag_str},
										 {HELP_COL, helps[0]},
									 });
		else
				print_columnized(ss, {
										 {SFLAG_COL, sflag_str},
										 {LFLAG_COL, lflag_str},
										 {HELP_COL, " "},
									 });
		if (helps.size()) helps.pop_front();
		for (const auto& help : helps) {
				print_columnized(ss, {
										 {SFLAG_COL, " "},
										 {LFLAG_COL, " "},
										 {HELP_COL, help},
									 });
		}
		if (opt.value()) {
				int max_rest = 0;
				for (const auto& rest : opt.value()->restrictions())
						max_rest = std::max(
							max_rest, static_cast<int>(rest.first.size()));
				const int help_col =
					line_width - max_rest - SUPPORTED_VALUE_INDENTATION - 2;

				for (const auto& rest : opt.value()->restrictions()) {
						print_columnized(
							ss,
							{
								{SUPPORTED_VALUE_INDENTATION, " "},
								{max_rest + 2, std::string(rest.first) + ": "},
								{help_col, std::string(rest.second)},
							});
				}
		}
		return;
}

void print_opts(std::stringstream& ss, const Opts& opts, int terminal_width) {
		int max_lflag_len = 0;
		for (const auto& pair : opts) {
				const auto& [meta, opt] = pair;
				const auto& [sflag, lflag] = meta;
				max_lflag_len =
					std::max(max_lflag_len, static_cast<int>(lflag.size()));
		}
		if (opts.size()) ss << "Options:\n";
		for (const auto& pair : opts) {
				print_opt(ss, pair, max_lflag_len, terminal_width);
		}
		return;
}

void print_pos_args(std::stringstream& ss, const PosArgs& pargs,
					int terminal_width) {
		if (!pargs.value()) return;
		const int INDENTATION = 4;
		int max_arg_len = 0;
		for (const auto& pair : pargs.value()->restrictions()) {
				max_arg_len =
					std::max(max_arg_len, static_cast<int>(pair.first.size()));
		}
		if (pargs.value()->restrictions().size()) {
				ss << "Subcommands:\n";
				for (const auto& pair : pargs.value()->restrictions()) {
						auto helps = split_to_lines(
							pair.second,
							terminal_width - INDENTATION - max_arg_len - 2);
						if (helps.size())
								print_columnized(
									ss, {
											{INDENTATION, " "},
											{max_arg_len + 2,
											 std::string(pair.first) + ": "},
											{terminal_width - INDENTATION -
												 max_arg_len - 2,
											 helps[0]},
										});
						else
								print_columnized(
									ss, {
											{INDENTATION, " "},
											{max_arg_len + 2,
											 std::string(pair.first) + ": "},
											{terminal_width - INDENTATION -
												 max_arg_len - 2,
											 " "},
										});
						helps.pop_front();
						for (const auto& help : helps) {
								print_columnized(
									ss,
									{
										{INDENTATION + max_arg_len + 2, " "},
										{terminal_width - INDENTATION -
											 max_arg_len - 2,
										 help},
									});
						}
				}
		}
}
}  // namespace

std::string helpgen(const Cmd& cmd, std::string brief_help,
					std::string_view usage, int terminal_width) {
		std::stringstream out{};
		print_brief(out, brief_help);
		print_usage(out, usage);
		print_opts(out, cmd.opts(), terminal_width);
		out << '\n';
		print_pos_args(out, cmd.pos_args(), terminal_width);
		return out.str();
}

}  // namespace cli
}  // namespace upp