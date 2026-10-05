#include <gtest/gtest.h>

#include <upp/rest/path_pattern.hpp>

using upp::rest::parse_path;
using upp::rest::path_pattern;

auto to_map(const auto& arr) {
    std::map<std::string_view, std::string_view> map{};
    for (auto [key, val] : arr) { map[key] = val; }
    return map;
}

TEST(Match, Empty) {
    auto res = parse_path(path_pattern<"">(), "");
    ASSERT_TRUE(res);
    auto arr = *std::move(res);
    static_assert(arr.size() == 0);
}

TEST(Match, FullMatch) {
    auto res = parse_path(path_pattern<"{foo}">(), "bar");
    ASSERT_TRUE(res);
    auto a = *std::move(res);
    static_assert(a.size() == 1);
    auto map = to_map(a);
    ASSERT_EQ(map["foo"], "bar");
}

TEST(Match, PrefixMatch) {
    auto res = parse_path(path_pattern<"{foo}/bar">(), "baz/bar");
    ASSERT_TRUE(res);
    auto a = *std::move(res);
    static_assert(a.size() == 1);
    auto map = to_map(a);
    ASSERT_EQ(map["foo"], "baz");
}

TEST(Match, SuffixMatch) {
    auto res = parse_path(path_pattern<"baz/{foo}">(), "baz/bar");
    ASSERT_TRUE(res);
    auto a = *std::move(res);
    static_assert(a.size() == 1);
    auto map = to_map(a);
    ASSERT_EQ(map["foo"], "bar");
}

TEST(Match, MidMatch) {
    auto res = parse_path(path_pattern<"foo/{key}/baz">(), "foo/bar/baz");
    ASSERT_TRUE(res);
    auto a = *std::move(res);
    static_assert(a.size() == 1);
    auto map = to_map(a);
    ASSERT_EQ(map["key"], "bar");
}

TEST(Match, Multiple) {
    auto res = parse_path(path_pattern<"{a}/{b}/{c}/{d}">(), "foo/bar/baz/asd");
    ASSERT_TRUE(res);
    auto a = *std::move(res);
    static_assert(a.size() == 4);
    auto map = to_map(a);
    ASSERT_EQ(map["a"], "foo");
    ASSERT_EQ(map["b"], "bar");
    ASSERT_EQ(map["c"], "baz");
    ASSERT_EQ(map["d"], "asd");
}

TEST(Match, NoKeys) {
    auto res = parse_path(path_pattern<"a/b">(), "a/b");
    ASSERT_TRUE(res);
    auto a = *std::move(res);
    static_assert(a.size() == 0);
}

TEST(Mismatch, NoKeys) {
    auto res = parse_path(path_pattern<"a/c">(), "a/b");
    ASSERT_FALSE(res);
}

TEST(Mismatch, WrongPrefix) {
    auto res = parse_path(path_pattern<"aa/{key}">(), "a/b");
    ASSERT_FALSE(res);
}

TEST(Mismatch, WrongSuffix) {
    auto res = parse_path(path_pattern<"a/{key}/cc">(), "a/b/c");
    ASSERT_FALSE(res);
}

TEST(Mismatch, MultiplePartsForKey) {
    auto res = parse_path(path_pattern<"prefix/{key}/suffix">(),
                          "prefix/resource/sub/suffix");
    ASSERT_FALSE(res);
}
