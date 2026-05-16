#include <gtest/gtest.h>

#include <upp/cleanup.hpp>

TEST(Cleanup, Simple) {
    bool cleaned_up = false;
    {
        auto c = upp::cleanup([&] { cleaned_up = true; });
        ASSERT_FALSE(cleaned_up);
    }
    ASSERT_TRUE(cleaned_up);
}

TEST(Cleanup, Throw) {
    bool cleaned_up = false;
    try {
        auto c = upp::cleanup([&] { cleaned_up = true; });
        ASSERT_FALSE(cleaned_up);
        throw std::runtime_error("asdf");
    } catch (...) {}
    ASSERT_TRUE(cleaned_up);
}

TEST(CleanupThrow, NoThrow) {
    bool cleaned_up = false;
    {
        auto c = upp::cleanup_throw([&] { cleaned_up = true; });
    }
    ASSERT_FALSE(cleaned_up);
}

TEST(CleanupThrow, Throw) {
    bool cleaned_up = false;
    try {
        auto c = upp::cleanup_throw([&] { cleaned_up = true; });
        ASSERT_FALSE(cleaned_up);
        throw std::runtime_error("asdf");
    } catch (...) {}
    ASSERT_TRUE(cleaned_up);
}

TEST(CleanupSuccess, NoThrow) {
    bool cleaned_up = false;
    {
        auto c = upp::cleanup_success([&] { cleaned_up = true; });
    }
    ASSERT_TRUE(cleaned_up);
}

TEST(CleanupSuccess, Throw) {
    bool cleaned_up = false;
    try {
        auto c = upp::cleanup_success([&] { cleaned_up = true; });
        ASSERT_FALSE(cleaned_up);
        throw std::runtime_error("asdf");
    } catch (...) {}
    ASSERT_FALSE(cleaned_up);
}
TEST(Cleanup, Cancel) {
    bool cleaned_up = false;
    {
        auto c = upp::cleanup([&] { cleaned_up = true; });
        c.cancel();
    }
    ASSERT_FALSE(cleaned_up);
}
