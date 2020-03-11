#include "upp/cli/parser.hpp"
using namespace upp::cli;

int main(int argc, const char** argv) {
    Parser<int> p("Demo program",
                  [](int* ptr, const Parser<int>::ParsingData& parsed) {
                      std::cout << "callback called" << std::endl;
                      return 0;
                  },
                  nullptr);
    // Parser<int> p(nullptr, nullptr, "Demo program");
    p.add_bool_options('f', "flag", "Demo flag",              //
                       'd', "debug", "Example debug option",  //
                       "demo", "This is a \"demo\" flag");

    p.add_options('a', "aaaaa", "AAA option",  //
                  "no-short", "This option has no short option");

    p.add_vector_options('v', "vector-option", "This is a vector option");

    p.add_subcommand("subcmd", "This is a hepl str",
                     [](int* ptr, const Parser<int>::ParsingData& parsed) {
                         std::cout << "Subcommand callback called" << std::endl;
                         return 0;
                     });
    p.parse(argc, argv);
}