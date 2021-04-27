#pragma once

#include <gtest/gtest.h>

#include "upp/parser.hpp"

class ParserFixture: public testing::Test {
public:
	ParserFixture(){}
	
	upp::parser::Factory<std::string::const_iterator> factory{};
};
