#include <gtest/gtest.h>

#include <upp/fs/tmpfile.hpp>
#include <upp/strlit.hpp>

using namespace upp::literals;
TEST(File, CreateTmpFile) {
    auto f = upp::fs::unnamed_tmpfile();
    (void)f;
}

TEST(File, RoundTrip) {
    auto input = "foobar"_lit;
    auto f = upp::fs::unnamed_tmpfile();
    f.write(input);
    f.seek_begin();
    auto buf = std::string(input.size(), '\0');
    auto count = f.read(buf);
    buf.resize(count);
    ASSERT_EQ(buf, input);
}

TEST(File, SeekEnd) {
    auto input = "foobar"_lit;
    auto f = upp::fs::unnamed_tmpfile();
    f.write(input);
    f.seek_begin();
    auto size = f.seek_end();
    ASSERT_EQ(size, input.size());
}

TEST(File, SeekCurr) {
    auto input = "foobar"_lit;
    auto f = upp::fs::unnamed_tmpfile();
    f.write(input);
    auto offset = f.seek_current(-1);
    ASSERT_EQ(offset, input.size() - 1);
}
