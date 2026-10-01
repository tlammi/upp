#include <gtest/gtest.h>

#include <upp/linux/pipe.hpp>
#include <upp/unused.hpp>

namespace ul = upp::linux;

using namespace std::literals;

TEST(Pipe, Open) {
    auto [read, write] = ul::pipe();
    upp::unused(read, write);
}

TEST(Pipe, Roundtrip) {
    auto [read, write] = ul::pipe();
    write.write("foobar");
    auto buf = std::array<char, 7>();
    read.read(buf);
    ASSERT_EQ(buf.data(), "foobar"sv);
}
