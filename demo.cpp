#include "upp/cli/value.hpp"
using namespace upp::cli;

int main() { Value{"-2147483649"}.as<int>(); }