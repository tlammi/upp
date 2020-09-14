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

		cli::Enum<SubCmd> subcmd{{SubCmd::SubCmd1, "subcmd1"},
								 {SubCmd::SubCmd2, "subcmd2"}};

		enum class EnumVal {
				A,
				B,
				C,
		};
		cli::Enum<EnumVal> enumeration{
			{EnumVal::A, "A"}, {EnumVal::B, "B"}, {EnumVal::C, "C"}};

		upp::cli::Cmd cmd{argv[0], "Demo CLI argument parser"};
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
		/*
				enum class Enumeration {
						A,
						B,
						C,
				};

				cli::Cmd cmd;
				cli::Enum<Enumeration> e{{Enumeration::A, "A"},
										 {Enumeration::B, "B"},
										 {Enumeration::C, "C"}};

				std::vector<int> ints{};

				double floating;
				std::string str{};

				std::vector<std::string> pos_args{};
				cmd.opts()
					.create('v', "vector")
					.store_in(ints)
					.set_help("Vector option");
				cmd.opts().create("enum").store_in(e).set_help("Demo
		   enumeration");
				cmd.opts().create("string").store_in(str).set_help("Demo
		   string"); cmd.opts() .create('f', "float") .store_in(floating)
					.set_help("Demo floating point");
				cmd.opts()
					.create('h', "help")
					.callback([&]() {
							std::cerr << "A program demostrating upp command
		   line " "parser\n"
									  << "\n"
									  << "Usage: " << argv[0]
									  << " [options] [positional arguments]\n"
									  << '\n'
									  << "Options:\n";
							for (const auto& conf : cmd.opts()) {
									std::cerr << '\t';
									if (conf.short_flag)
											std::cerr << "-" << conf.short_flag;
									std::cerr << '\t';
									if (conf.long_flag)
											std::cerr << "--" << conf.long_flag;
									std::cerr << '\t';
									std::cerr << conf.opt.help() << '\n';
							}

							throw cli::HelpException("");
					})
					.set_help("Print this help");

				cmd.pos_args().store_in(pos_args);

				try {
						cmd.parse(argv + 1, argv + argc);
				} catch (const cli::HelpException&) { exit(1); }
				char** iter = argv;
				std::cerr << "Arguments: \n";
				while (iter != argv + argc) {
						std::cerr << '\t' << *iter << '\n';
						++iter;
				}

				if (cmd.opts()['v']) std::cerr << "Vector values:";
				for (const auto& i : ints) { std::cerr << ' ' << i; }
				if (cmd.opts()["vector"]) std::cerr << '\n';

				if (cmd.opts()["enum"]) std::cerr << "Enumeration: " << e.str()
		   << '\n'; if (cmd.opts()["string"]) std::cerr << "String: " << str <<
		   '\n'; if (cmd.opts()['f']) std::cerr << "Floating point: " <<
		   floating << '\n'; std::cerr << "Positional arguments:\n"; for (const
		   auto& arg : pos_args) { std::cerr << '\t' << arg << '\n'; }
				*/
}