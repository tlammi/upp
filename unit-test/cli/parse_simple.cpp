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
		Cmd cmd{"asdf"};
		cmd.opts().create('h', "help");
		cmd.opts().create("option");
		cmd.opts().create('s');
		ASSERT_FALSE(cmd.opts()["help"]);
		ASSERT_FALSE(cmd.opts()["option"]);
		ASSERT_FALSE(cmd.opts()['s']);
		const char** out = parse(cmd, args + 1, args + 3);
		ASSERT_EQ(out, args + 3);
		ASSERT_TRUE(cmd.opts()['h']);
		ASSERT_TRUE(cmd.opts()["option"]);
		ASSERT_FALSE(cmd.opts()['s']);
}

TEST(CliTest, SingleValue) {
		const char* args[] = {"app", "-o", "value"};

		std::string val;
		Cmd cmd{"app"};
		cmd.opts().create('o').store_in(val);
		const char** out = parse(cmd, args + 1, args + 3);
		ASSERT_EQ(out, args + 3);
		ASSERT_EQ(val, "value");
}