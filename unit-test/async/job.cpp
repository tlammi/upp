#include "upp/impl/async/job.hpp"

#include <gtest/gtest.h>

#include "upp/impl/async/fiberexecutor.hpp"
#include "upp/impl/async/fifoexecutor.hpp"

using namespace upp::async;
using namespace std::literals::chrono_literals;

int intf(int i) { return i * 2; }

void voidf() {}

TEST(AsyncJobTest, Ctor) {
		FifoExecutor<1> exec{};
		Job j0{exec, intf};
		Job j1{exec, [](double d) { return d * d; }};
		Job j2{exec, voidf};
}

TEST(AsyncJobTest, Run) {
		FifoExecutor<1> exec{};

		int i = 1;
		Job j0{exec, [&](int j) mutable {
					   i = j;
					   return j * 2;
			   }};
		Job j1{exec, [&](int j) { return i + j; }};
		exec.start();
		auto fut0 = j0(100);
		auto fut1 = j1(1000);

		fut0.wait();
		fut1.wait();

		ASSERT_EQ(i, 100);
		ASSERT_EQ(fut0.get(), 200);
		ASSERT_EQ(fut1.get(), 1100);
}

TEST(AsyncJobTest, RunFiberExecutor) {
		FiberExecutor exec{};
		int i = 0;
		Job j0{exec, [&](int j) mutable {
					   i = j * 2;
					   return j * 3;
			   }};
		Job j1{exec, [&](int j) mutable { return i + j; }};

		auto f0 = j0(10);
		auto f1 = j1(2);

		exec.run_all();

		ASSERT_TRUE(f0.valid());
		ASSERT_TRUE(f1.valid());
		ASSERT_EQ(i, 20);
		ASSERT_EQ(f0.get(), 30);
		ASSERT_EQ(f1.get(), 22);
}

TEST(AsyncJobTest, OverridingJob) {
		FiberExecutor exec{};
		int i = 0;
		OverridingJob j0{exec, [&]() { i = 1; }};
		OverridingJob j1{exec, [&]() { i = 2; }};

		j0();
		j1();
		exec.run_all();
		ASSERT_EQ(i, 2);

		j1();
		j0();
		j1();
		j1();
		j0();
		exec.run_all();
		ASSERT_EQ(i, 1);
}

TEST(AsyncJobTest, ContinuousJob) {
		FifoExecutor<1> exec{};
		std::atomic<int> i = 0;
		ContinuousJob j0{exec, [&]() { ++i; }};
		exec.start();
		std::this_thread::sleep_for(10ms);
		exec.stop();

		ASSERT_GT(i, 0);
}
