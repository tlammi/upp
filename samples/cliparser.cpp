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
		};

		cli::Enum<SubCmd> subcmd{
			{SubCmd::SubCmd1, "subcmd1", "Run subcommand 1"},
			{SubCmd::SubCmd2, "subcmd2", "Run subcommand 2"},
		};

		enum class EnumVal {
				A,
				B,
				C,
		};
		cli::Enum<EnumVal> enumeration{
			{EnumVal::A, "A"}, {EnumVal::B, "B"}, {EnumVal::C, "C"}};

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
				std::cerr << upp::cli::helpgen(cmd);
				throw upp::cli::HelpException("");
		});
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