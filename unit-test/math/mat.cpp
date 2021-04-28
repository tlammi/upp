
#include <gtest/gtest.h>

#include "upp/math/mat.hpp"

namespace m = upp::math;

TEST(Mat, Idx){
	m::Mat<int, 1, 1> mat{};
	mat(0, 0) = 100;
	ASSERT_EQ(mat(0, 0), 100);
}


TEST(Mat, Add){
	m::Mat<int, 2, 2> m0{};
	m::Mat<int, 2, 2> m1{};

	m0(0, 0) = 1;
	m0(0, 1) = 2;
	m0(1, 0) = 3;
	m0(1, 1) = 4;

	m1(0, 0) = 2;
	m1(0, 1) = 4;
	m1(1, 0) = 6;
	m1(1, 1) = 8;

	ASSERT_EQ((m0+m1)(0, 0), 3);
	ASSERT_EQ((m0+m1)(0, 1), 6);
	ASSERT_EQ((m0+m1)(1, 0), 9);
	ASSERT_EQ((m0+m1)(1, 1), 12);
}


TEST(Mat, AddMultiple){
	m::Mat<int, 1, 1> m0{};
	m::Mat<int, 1, 1> m1{};
	m::Mat<int, 1, 1> m2{};

	m0(0, 0) = 1;
	m1(0, 0) = 10;
	m2(0, 0) = 100;
	
	m::Mat res = (m0 + m1 + m2);
	ASSERT_EQ(res(0, 0), 111);
}


TEST(Mat, Sub){
	m::Mat<int, 2, 2> m0{};
	m::Mat<int, 2, 2> m1{};

	m0(0, 0) = 1;
	m0(0, 1) = 2;
	m0(1, 0) = 3;
	m0(1, 1) = 4;

	m1(0, 0) = 2;
	m1(0, 1) = 4;
	m1(1, 0) = 6;
	m1(1, 1) = 8;

	ASSERT_EQ((m0-m1)(0, 0), -1);
	ASSERT_EQ((m0-m1)(0, 1), -2);
	ASSERT_EQ((m0-m1)(1, 0), -3);
	ASSERT_EQ((m0-m1)(1, 1), -4);
}

TEST(Mat, SubMultiple){
	m::Mat<int, 1, 1> m0{};
	m::Mat<int, 1, 1> m1{};
	m::Mat<int, 1, 1> m2{};

	m0(0, 0) = 1;
	m1(0, 0) = 10;
	m2(0, 0) = 100;

	m::Mat res = m0 - m1 - m2;
	ASSERT_EQ(res(0, 0), -109);
}

TEST(Mat, Equal){
	m::Mat<int, 1, 2> m0{};
	m::Mat<int, 1, 2> m1{};

	m0(0, 0) = 100;
	m0(0, 1) = 99;
	m1(0, 0) = 100;
	m1(0, 1) = 99;

	ASSERT_EQ(m0, m1);
}

TEST(Mat, NotEqual){
	m::Mat<int, 1, 2> m0{};
	m::Mat<int, 1, 2> m1{};

	m0(0, 0) = 100;
	m0(0, 1) = 99;
	m1(0, 0) = 100;
	m1(0, 1) = 98;

	ASSERT_NE(m0, m1);
}


TEST(Mat, Mul){
	m::Mat<int, 1, 2> m0{};
	m::Mat<int, 2, 1> m1{};

	m0(0, 0) = 1;
	m0(0, 1) = 2;
	m1(0, 0) = 3;
	m1(1, 0) = 4;

	ASSERT_EQ((m0*m1)(0, 0), 11);

}

TEST(Mat, MulMultiple){
	m::Mat<int, 1, 2> m0{};
	m::Mat<int, 2, 2> m1{};
	m::Mat<int, 2, 1> m2{};
	
	m0(0, 0) = 1;
	m0(0, 1) = 2;

	m1(0, 0) = 3;
	m1(0, 1) = 4;
	m1(1, 0) = 5;
	m1(1, 1) = 6;

	m2(0, 0) = 7;
	m2(1, 0) = 8;

	m::Mat res = m0*m1*m2;
	ASSERT_EQ(res(0, 0), 219);
}

TEST(Mat, Transpose){
	m::Mat<int, 1, 2> mat{};
	mat(0, 0) = 1;
	mat(0, 1) = 2;

	auto trans1 = m::transpose(mat);
	auto trans2 = mat.t();

	ASSERT_EQ(trans1, trans2);

	ASSERT_EQ(trans1(0, 0), 1);
	ASSERT_EQ(trans1(1, 0), 2);

}

TEST(Mat, DiagMat){

	m::DiagMat<int, 2, 3> d{};
	d(0) = 1;
	d(1) = 2;

	m::Mat<int, 3, 1> mat{};

	mat(0, 0) = 3;
	mat(1, 0) = 4;
	mat(2, 0) = 5;

	m::Mat<int, 2, 1> expected{};

	expected(0, 0) = 3;
	expected(1, 0) = 8;

	ASSERT_EQ((d*mat), expected);
}
