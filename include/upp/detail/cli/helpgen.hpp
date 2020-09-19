#pragma once
#include <string>

#include "upp/detail/cli/cmd.hpp"
namespace upp {
namespace cli {

std::string helpgen(const Cmd& cmd, std::string brief_help = "",
					std::string_view usage = "") {
		std::stringstream out{};

		if (brief_help != "") out << brief_help << "\n\n";
		if (usage != "") out << "Usage: " << usage << "\n\n";
		if (cmd.opts().size()) out << "Options:\n";
		for (const auto& [meta, opt] : cmd.opts()) {
				const auto& [sflag, lflag] = meta;
				out << " -" << sflag << " --" << lflag << "  " << opt.help();
				if (opt.support_multiple())
						out << " (Can be specified multiple times)";
				const auto rest = opt.value_restrictions();
				if (rest.size()) { out << " (Supported values:"; }
				for (const auto& pair : opt.value_restrictions()) {
						out << " \"" << pair.first << '"';
				}
				if (rest.size()) out << ')';
				out << '\n';
		}

		if (cmd.pos_args().value() &&
			cmd.pos_args().value()->support_multiple()) {
				out << "\nPositional Arguments:\n";
		} else if (cmd.pos_args().value() &&
				   cmd.pos_args().value()->value_restrictions().size()) {
				out << "\nSubcommands:\n";
				for (const auto& pair :
					 cmd.pos_args().value()->value_restrictions()) {
						out << '\t' << pair.first;
						if (pair.second != "") { out << ": " << pair.second; }
						out << '\n';
				}
		}
		out << '\n';
		return out.str();
}
}  // namespace cli
}  // namespace upp