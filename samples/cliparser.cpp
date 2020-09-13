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
		cmd.opts().create('v', "vector").store_in(ints);
		cmd.opts().create("enum").store_in(e);
		cmd.opts().create("string").store_in(str);
		cmd.opts().create('f', "float").store_in(floating);
		cmd.opts().create('h', "help").callback([&]() {
				std::cerr << "A program demostrating upp command line "
							 "parser\n"
						  << "\n"
						  << "Usage: " << argv[0]
						  << " [options] [positional arguments]\n"
						  << '\n'
						  << "Options:\n"
						  << '\t' << "-f --float    A floating point value\n"
						  << '\t' << "   --enum     An enumeration value\n"
						  << '\t' << "-h --help     Print this help and exit\n"
						  << '\t' << "   --string   String value\n"
						  << '\t'
						  << "-v --vector   Int option that can be specified "
							 "multiple times\n";

				throw cli::HelpException("");
		});
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