/* SPDX-License-Identifier: MIT */
/* Copyright @ 2020 Toni Lammi */
#pragma once
#include <deque>
#include <iomanip>
#include <string>

#include "upp/impl/cli/cmd.hpp"
#include "upp/impl/cli/opt.hpp"
namespace upp {
namespace cli {

/**
 * @brief Generate help for a command
 *
 * @param cmd Command for which the help is generated
 * @param brief_help Brief help string. Short program description placed first
 * in the help string
 * @param usage Usage placed in the help print. E.g. <cmd> [options] [arguments]
 *
 * @param terminal_width Terminal width, If >0, use this as a guide for clean
 * formatting. If <0, ignored
 * @return std::string String containing the help text
 */
std::string helpgen(const Cmd& cmd, std::string brief_help = "",
					std::string_view usage = "", int terminal_width = 0);

}  // namespace cli
}  // namespace upp