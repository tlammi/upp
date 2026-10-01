#include <fcntl.h>
#include <gtest/gtest.h>

#include <ranges>
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

TEST(Pipe, PacketMode) {
    auto [read, write] = ul::pipe(O_DIRECT);
    write.write("foo");
    write.write("bar");

    std::string buffer(10, '\0');
    auto count = read.read(buffer);
    auto view = std::string_view(buffer).substr(0, count - 1);
    ASSERT_EQ(view, "foo");
    count = read.read(buffer);
    view = std::string_view(buffer).substr(0, count - 1);
    ASSERT_EQ(view, "bar");
}

TEST(Pipe, PacketTooLarge) {
    auto [read, write] = ul::pipe(upp::bm | ul::pipe_flag::packet);

    static constexpr std::size_t elem_count = PIPE_BUF + PIPE_BUF / 2;

    auto write_buf = std::array<std::byte, elem_count>();
    write.write(write_buf);

    auto buf = std::string(elem_count, '\0');

    auto count = read.read(buf);
    ASSERT_EQ(count, PIPE_BUF);
    count = read.read(buf);
    ASSERT_LT(count, PIPE_BUF);
}
