#include <gtest/gtest.h>

#include <upp/net.hpp>

TEST(Parse, Root) {
    auto res = upp::net::parse_route("/", "/");
    ASSERT_TRUE(res);
    ASSERT_TRUE(res->params().empty());
}

TEST(Parse, ParamMatchEnd) {
    auto res = upp::net::parse_route("/{foo}", "/bar");
    ASSERT_TRUE(res);
    ASSERT_EQ(res->params().at("foo"), "bar");
}

TEST(Parse, ParamMatchBegin) {
    auto res = upp::net::parse_route("{foo}/", "bar/");
    ASSERT_TRUE(res);
    ASSERT_EQ(res->params().at("foo"), "bar");
}

TEST(Parse, ParamMatchMiddle) {
    auto res = upp::net::parse_route("/a/{foo}/b", "/a/bar/b/");
    ASSERT_TRUE(res);
    ASSERT_EQ(res->params().at("foo"), "bar");
}

TEST(Parse, Multiple) {
    auto res = upp::net::parse_route("/{a}/{b}/{c}/{d}", "/AAA/BBB/CCC/DDD");
    ASSERT_TRUE(res);
    const auto& params = res->params();
    ASSERT_EQ(params.at("a"), "AAA");
    ASSERT_EQ(params.at("b"), "BBB");
    ASSERT_EQ(params.at("c"), "CCC");
    ASSERT_EQ(params.at("d"), "DDD");
}

TEST(Parse, PrefixMissmatch) {
    auto res = upp::net::parse_route("/foo/{a}", "/bar/baz");
    ASSERT_FALSE(res);
}
