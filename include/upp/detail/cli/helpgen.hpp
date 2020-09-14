#pragma once
#include <string>

#include "upp/detail/cli/cmd.hpp"
namespace upp {
namespace detail {
namespace cli {

std::string helpgenimpl(const Cmd& cmd) {
		std::stringstream out{};
		out << cmd.help() << "\n\nUsage: " << cmd.usage() << '\n' << '\n';
		if (cmd.opts().size()) out << "Options:\n";
		for (const auto& [sflag, lflag, opt] : cmd.opts()) {
				out << " -" << sflag << " --" << lflag << "  " << opt.help();
				if (opt.support_multiple_values())
						out << " (Multiple values can be specified)";
				const auto rest = opt.value_restrictions();
				if (rest.size()) { out << " (Supported values:"; }
				for (const auto& limit : opt.value_restrictions()) {
						out << " \"" << limit << '"';
				}
				if (rest.size()) out << ')';
				out << '\n';
		}
		return out.str();
}
}  // namespace cli
}  // namespace detail
}  // namespace upp