/**
 * @file cliparser.cpp Example for using upp::cli for parsing command line
 * arguments
 *
 * @author tlammi (tlammi@github.com)
 * @brief
 * @version 0.1
 * @date 2020-09-19
 *
 * @copyright Copyright (c) 2020
 *
 */
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <iostream>

#include "upp/cli.hpp"

namespace cli = upp::cli;

void parse_subcmd1(char** iter, char** end) {
		upp::cli::Cmd cmd{};
		cmd.opts()
			.create('h', "help")
			.set_help("Print this help and exit")
			.callback([&]() { std::cerr << "This helps\n"; });
		if (cmd.parse(iter, end) != end)
				throw std::invalid_argument("Too many arguments");
}

int main(int argc, char** argv) {
		enum class SubCmd {
				SubCmd1,
				SubCmd2,
				SubCmd3,
		};

		cli::Enum<SubCmd> subcmd{
			{SubCmd::SubCmd1, "subcmd1", "Run subcommand 1"},
			{SubCmd::SubCmd2, "subcmd2", "Run subcommand 2"},
			{SubCmd::SubCmd3, "subcmd3",
			 "This subcommand has a really long "
			 "help........................................."
			 "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa..........."},
		};

		enum class EnumVal {
				A,
				B,
				C,
		};
		cli::Enum<EnumVal> enumeration{
			{EnumVal::A, "A", "An option that has a help"},
			{EnumVal::B, "B"},
			{EnumVal::C, "C"}};

		upp::cli::Cmd cmd{};
		std::vector<int> vect;
		cmd.opts()
			.create('v', "vector")
			.set_help("Vector option")
			.store_in(vect);
		cmd.opts()
			.create('e', "enum")
			.set_help("Sample enumeration")
			.store_in(enumeration);
		cmd.opts().create('h', "help").callback([&]() {
				winsize ws;
				ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
				std::cerr << upp::cli::helpgen(cmd, "Example CLI parser",
											   "[options] <subcmd>", ws.ws_col);
				throw upp::cli::HelpException("");
		});

		cmd.opts()
			.create('l', "long")
			.set_help(
				"Option with realllllllllllly long "
				"flag.................................................."
				"........"
				"....................................................."
				".");
		cmd.pos_args().store_in(subcmd);
		char** iter;
		try {
				iter = cmd.parse(argv + 1, argv + argc);
		} catch (const upp::cli::HelpException&) { exit(1); }

		switch (subcmd) {
				case SubCmd::SubCmd1: {
						parse_subcmd1(iter, argv + argc);
						break;
				}
				case SubCmd::SubCmd2: {
						std::cerr << "SubCmd2\n";
						break;
				}
				default: {
						throw std::invalid_argument("Invalid subcmd");
				}
		}
}