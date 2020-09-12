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
		const char** out = cmd.parse(args + 1, args + 3);
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
		const char** out = cmd.parse(args + 1, args + 3);
		ASSERT_EQ(out, args + 3);
		ASSERT_EQ(val, "value");
}

TEST(CliTest, MultipleValues) {
		const char* args[] = {"app", "-v", "1", "-v", "2", "-v", "4"};
		Cmd cmd{"app"};
		std::vector<std::string> vect;
		cmd.opts().create('v').store_in(vect);
		const char** out = cmd.parse(args + 1, args + 7);

		ASSERT_EQ(vect.size(), 3);
		ASSERT_EQ(vect[0], "1");
		ASSERT_EQ(vect[1], "2");
		ASSERT_EQ(vect[2], "4");
}

TEST(CliTest, Enum) {
		enum class Demo {
				A,
				B,
				C,
		};
		Enum<Demo> e{{Demo::A, "A"}, {Demo::B, "B"}, {Demo::C, "C"}};
		const char* args[] = {"app", "--flag", "C"};

		Cmd cmd{"app"};
		cmd.opts().create("flag").store_in(e);
		cmd.parse(args + 1, args + 3);

		ASSERT_EQ(e, Demo::C);
		ASSERT_NE(e, Demo::B);
}
