#include "upp/cli/parser.hpp"
using namespace upp::cli;

int main(int argc, const char** argv) {
    Parser<int> p(nullptr, nullptr, "Demo program");
    p.add_bool_option('f', "flag", "Demo flag");
    p.add_option('a', "aaaaa", "AAA option");
    p.add_vector_option('v', "vector-option", "This is a vector option");
    p.add_option("no-short", "This option has no short flag");
    p.parse(argc, argv);
}