#include <gtest/gtest.h>

#include <upp/fs/tmpfile.hpp>

namespace fs = std::filesystem;
TEST(Create, Default) {
    auto t = upp::fs::tmpdir();
    (void)t;
}

TEST(Deletion, Auto) {
    auto path = fs::path();
    {
        auto t = upp::fs::tmpdir();
        path = t.path() / "sample.txt";
        auto f = upp::fs::file(path);
        ASSERT_TRUE(fs::exists(path));
    }
    ASSERT_FALSE(fs::exists(path));
}
