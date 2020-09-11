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
		cmd.opts().create('v', "vector").store_in(ints);
		cmd.opts().create("enum").store_in(e);
		cli::parse(cmd, argv + 1, argv + argc);
		std::cerr << "Vector values:";
		for (const auto& i : ints) { std::cerr << ' ' << i; }
		std::cerr << '\n';

		if (cmd.opts()["enum"]) std::cerr << "Enumeration: " << e.str() << '\n';
}