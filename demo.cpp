#include "upp/cli/parser.hpp"
using namespace upp::cli;

int main(int argc, const char** argv) {
    Parser<int> p(nullptr, nullptr, "Demo program");
    p.boolopts().add('f', "flag", "Demo flag");
    p.opts().add('a', "aaaaa", "AAAA option");
    p.parse(argc, argv);
}