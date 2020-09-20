#pragma once
#include <deque>
#include <iomanip>
#include <string>

#include "upp/impl/cli/cmd.hpp"
#include "upp/impl/cli/opt.hpp"
namespace upp {
namespace cli {
std::string helpgen(const Cmd& cmd, std::string brief_help = "",
					std::string_view usage = "", int terminal_width = 0);

}  // namespace cli
}  // namespace upp