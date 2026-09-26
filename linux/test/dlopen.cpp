#include <gtest/gtest.h>

#include <filesystem>
#include <upp/linux/dlopen.hpp>
#include <upp/unused.hpp>

using upp::linux::dynamic_library;

TEST(Init, Default) {
    auto dl = dynamic_library();
    ASSERT_FALSE(dl);
}

TEST(Init, PathThatDoesNotExist) {
    ASSERT_ANY_THROW(dynamic_library("/path/that/should/not/exist"));
}

auto setup_dynlib() {
    const char* builddir = std::getenv("BUILD_DIR");
    assert(builddir != nullptr && "BUILD_DIR environment variable not set");
    auto path = std::filesystem::path(builddir) / "libdlopentest.so";
    return dynamic_library(path.native().c_str());
}

TEST(Init, ExistingLib) {
    auto dl = setup_dynlib();
    upp::unused(dl);
}

TEST(Symbol, NonExisting) {
    auto dl = setup_dynlib();
    ASSERT_ANY_THROW(dl.raw_symbol("does_not_exist"));
}

TEST(Symbol, Function) {
    auto dl = setup_dynlib();
    auto* fn = dl.symbol<int (*)()>("meaning_of_life");
    ASSERT_TRUE(fn);
    ASSERT_EQ(fn(), 42);
}

TEST(Symbol, Constant) {
    auto dl = setup_dynlib();
    auto* pi = dl.symbol<const int*>("APPROX_PI");
    ASSERT_TRUE(pi);
    ASSERT_EQ(*pi, 4);
}

TEST(Symbol, Roundtrip) {
    auto dl = setup_dynlib();
    auto* variable = dl.symbol<int*>("VARIABLE");
    auto* fn = dl.symbol<int (*)()>("get_variable");
    *variable = 1;
    ASSERT_EQ(fn(), 1);
    *variable = 2;
    ASSERT_EQ(fn(), 2);
}

TEST(Symbol, Args) {
    auto dl = setup_dynlib();
    auto* add = dl.symbol<int (*)(int, int)>("add");
    auto res = add(1, 2);
    ASSERT_EQ(res, 3);
    res = add(3, 4);
    ASSERT_EQ(res, 7);
}
