#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <upp/math/mat.hpp>

using upp::math::mat;
using upp::math::row;
using upp::math::static_mat;

using ::testing::ElementsAre;

TEST(Init, Zero) {
    auto m = mat<int>::zeros(2, 3);
    ASSERT_EQ(m.height(), 2);
    ASSERT_EQ(m.width(), 3);
    auto rows = m.rows();
    ASSERT_THAT(rows[0], ElementsAre(0, 0, 0));
    ASSERT_THAT(rows[1], ElementsAre(0, 0, 0));
}

TEST(Init, Identity) {
    auto m = mat<int>::eye(2, 3);
    ASSERT_EQ(m.height(), 2);
    ASSERT_EQ(m.width(), 3);
    auto cols = m.columns();
    ASSERT_THAT(cols[0], ElementsAre(1, 0));
    ASSERT_THAT(cols[1], ElementsAre(0, 1));
    ASSERT_THAT(cols[2], ElementsAre(0, 0));
}

TEST(Init, Rows) {
    auto m = mat<int>(row{1, 2, 3}, row{2, 3, 4});
    ASSERT_EQ(m.height(), 2);
    ASSERT_EQ(m.width(), 3);
    auto cols = m.columns();
    ASSERT_THAT(cols[0], ElementsAre(1, 2));
    ASSERT_THAT(cols[1], ElementsAre(2, 3));
    ASSERT_THAT(cols[2], ElementsAre(3, 4));
    auto rows = m.rows();
    ASSERT_THAT(rows[0], ElementsAre(1, 2, 3));
    ASSERT_THAT(rows[1], ElementsAre(2, 3, 4));
}

TEST(Modify, Indexing) {
    auto m = mat<int>::zeros(2, 2);
    m[0, 0] = 3;
    auto v = m[0, 0];
    ASSERT_EQ(v, 3);
}

TEST(Modify, Rows) {
    auto m = mat<int>::zeros(2, 2);
    m.rows()[0][1] = 2;
    auto c = m.columns();
    ASSERT_EQ(c[1][0], 2);
}

TEST(Ops, AddAssignOk) {
    auto a = mat<int>(row{1, 2}, row{3, 4});
    auto b = mat<int>(row{1, 2}, row{3, 4});
    a += b;
    auto rows = a.rows();
    ASSERT_THAT(rows[0], ElementsAre(2, 4));
    ASSERT_THAT(rows[1], ElementsAre(6, 8));
}

TEST(Ops, AddAssignDimsMismatch) {
    auto a = mat<int>(row{1, 2});
    auto b = mat<int>(row{1});
    ASSERT_ANY_THROW(a += b);
}

TEST(Ops, SubAssignOk) {
    auto a = mat<int>(row{1, 2}, row{3, 4});
    auto b = mat<int>(row{1, 2}, row{3, 4});
    a -= b;
    auto rows = a.rows();
    ASSERT_THAT(rows[0], ElementsAre(0, 0));
    ASSERT_THAT(rows[1], ElementsAre(0, 0));
}

TEST(Ops, SubAssignDimsMismatch) {
    auto a = mat<int>(row{1, 2});
    auto b = mat<int>(row{1});
    ASSERT_ANY_THROW(a -= b);
}

TEST(Ops, MulAssignOk) {
    auto a = mat<int>(row{1, 2}, row{3, 4});
    auto b = mat<int>(row{1, 2}, row{3, 4});
    a *= b;
    auto rows = a.rows();
    ASSERT_THAT(rows[0], ElementsAre(7, 10));
    ASSERT_THAT(rows[1], ElementsAre(15, 22));
}

TEST(Ops, MulAssignDimsMismatch) {
    auto a = mat<int>(row{1, 2}, row{3, 4});
    auto b = mat<int>(row{1, 2});
    ASSERT_ANY_THROW(a *= b);
}

TEST(StaticInit, Default) {
    auto s = static_mat<int, 3, 3>();
    auto v = s[0, 0];
    ASSERT_EQ(v, 0);
}

TEST(StaticInit, Rows) {
    // NOLINTNEXTLINE(*magic-number*)
    auto s = make_static_mat<int>(row{1, 2}, row{3, 4}, row{5, 6});
    ASSERT_EQ(s.height(), 3);
    ASSERT_EQ(s.width(), 2);

    auto rows = s.rows();
    ASSERT_THAT(rows[0], ElementsAre(1, 2));
    ASSERT_THAT(rows[1], ElementsAre(3, 4));
    ASSERT_THAT(rows[2], ElementsAre(5, 6));

    auto cols = s.columns();

    ASSERT_THAT(cols[0], ElementsAre(1, 3, 5));
    ASSERT_THAT(cols[1], ElementsAre(2, 4, 6));
}

TEST(Ops, AddStatics) {
    auto a = make_static_mat<int>(row{1, 2});
    auto b = make_static_mat<int>(row{2, 3});
    auto c = a + b;
    auto c00 = c[0, 0];
    auto c01 = c[0, 1];
    ASSERT_EQ(c00, 3);
    ASSERT_EQ(c01, 5);
}
