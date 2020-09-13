#include <iostream>

#include "upp/cli.hpp"

namespace cli = upp::cli;

int main(int argc, char** argv) {
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
		cmd.opts().create("enum").store_in(e).set_help("Demo enumeration");
		cmd.opts().create("string").store_in(str).set_help("Demo string");
		cmd.opts()
			.create('f', "float")
			.store_in(floating)
			.set_help("Demo floating point");
		cmd.opts()
			.create('h', "help")
			.callback([&]() {
					std::cerr << "A program demostrating upp command line "
								 "parser\n"
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

		if (cmd.opts()["enum"]) std::cerr << "Enumeration: " << e.str() << '\n';
		if (cmd.opts()["string"]) std::cerr << "String: " << str << '\n';
		if (cmd.opts()['f'])
				std::cerr << "Floating point: " << floating << '\n';
		std::cerr << "Positional arguments:\n";
		for (const auto& arg : pos_args) { std::cerr << '\t' << arg << '\n'; }
}