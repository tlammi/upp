#include <fcntl.h>
#include <gtest/gtest.h>

#include <upp/linux/pipe.hpp>
#include <upp/linux/run.hpp>

using namespace std::literals;

TEST(Simple, True) {
    auto res = upp::linux::run({"true"});
    ASSERT_EQ(res, 0);
}

TEST(Simple, False) {
    auto res = upp::linux::run({"false"});
    ASSERT_EQ(res, 1);
}

TEST(Simple, Args) {
    auto res = upp::linux::run({"sh", "-c", "exit 2"});
    ASSERT_EQ(res, 2);
}

TEST(Simple, Dynamic) {
    auto args = std::vector<std::string>{"sh", "-c", "exit 3"};
    auto res = upp::linux::run(args);
    ASSERT_EQ(res, 3);
}

TEST(Io, Stdout) {
    auto pipe = upp::linux::pipe();
    auto res = upp::linux::run({"echo", "-n", "foo"},
                               {.std_out = std::move(pipe.write)});
    ASSERT_EQ(res, 0);
    auto buf = std::string(100, '\0');
    auto count = pipe.read.read(buf);
    buf = buf.substr(0, count);
    ASSERT_EQ(buf, "foo");
}

TEST(Io, Stderr) {
    auto pipe = upp::linux::pipe();
    auto res = upp::linux::run({"sh", "-c", "echo -n bar >&2"},
                               {.std_err = std::move(pipe.write)});
    ASSERT_EQ(res, 0);
    auto buf = std::string(100, '\0');
    auto count = pipe.read.read(buf);
    buf = buf.substr(0, count);
    ASSERT_EQ(buf, "bar");
}

TEST(Io, Stdin) {
    auto in = upp::linux::pipe();
    auto out = upp::linux::pipe();

    in.write.write("token"sv);
    in.write.close();  // Send end-of-stream
    auto res = upp::linux::run(
        {
            "cat",
        },
        {.std_in = std::move(in.read), .std_out = std::move(out.write)});
    ASSERT_EQ(res, 0);
    auto buf = std::string(100, '\0');
    auto count = out.read.read(buf);
    buf = buf.substr(0, count);
    ASSERT_EQ(buf, "token");
}
