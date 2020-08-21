#include <gtest/gtest.h>

#include "upp/cli.hpp"

using namespace upp::cli;

TEST(CliTest, Init) {
		Cmd cmd{"asdf"};
		cmd.opts().create('h', "hello");
		cmd.opts().create("option");
		cmd.opts().create('s');
}

TEST(CliTest, NoValue) {
		const char* args[] = {"demo", "-h", "--option"};
		const char** end = args + 3;
		Cmd cmd{"asdf"};
		cmd.opts().create('h', "help");
		cmd.opts().create("option");
		cmd.opts().create('s');
		ASSERT_FALSE(cmd.opts()["help"]);
		ASSERT_FALSE(cmd.opts()["option"]);
		ASSERT_FALSE(cmd.opts()['s']);
		const char** out = parse(cmd, args + 1, end);
		ASSERT_EQ(out, end);
		ASSERT_TRUE(cmd.opts()['h']);
		ASSERT_TRUE(cmd.opts()["option"]);
		ASSERT_FALSE(cmd.opts()['s']);
}