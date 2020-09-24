#include "upp/impl/async/job.hpp"

#include <gtest/gtest.h>

using namespace upp::async;

int intf(int i) { return i * 2; }

void voidf() {}

TEST(AsyncJobTest, Ctor) {
		Job j0{nullptr, intf};
		Job j1{nullptr, [](double d) { return d * d; }};
		Job j2{nullptr, voidf};
}

TEST(AsyncJobTest, Run) {
		FifoExecutor<1> exec{};

		int i = 1;
		Job j0{&exec, [&](int j) mutable {
					   i = j;
					   return j * 2;
			   }};
		Job j1{&exec, [&](int j) { return i + j; }};

		auto fut0 = j0(100);
		auto fut1 = j1(1000);

		fut0.wait();
		fut1.wait();

		ASSERT_EQ(i, 100);
		ASSERT_EQ(fut0.get(), 200);
		ASSERT_EQ(fut1.get(), 1100);
}