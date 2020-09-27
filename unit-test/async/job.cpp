#include "upp/impl/async/job.hpp"

#include <gtest/gtest.h>

#include "upp/impl/async/continuousjob.hpp"
#include "upp/impl/async/fiberexecutor.hpp"
#include "upp/impl/async/fifoexecutor.hpp"
#include "upp/impl/async/intervaljob.hpp"
#include "upp/impl/async/overridingjob.hpp"
#include "upp/impl/timer/oneshot.hpp"
using namespace upp::async;
using namespace std::literals::chrono_literals;

void intf(int i) { (void)i; }

void voidf() {}

TEST(AsyncJobTest, Ctor) {
		FifoExecutor<1> exec{};
		Job j0{exec, intf};
		Job j1{exec, [](double d) { (void)d; }};
		Job j2{exec, voidf};

		ContinuousJob j3{exec, voidf};

		OverridingJob j4{exec, intf};
		OverridingJob j5{exec, [](float f) { (void)f; }};
		OverridingJob j6{exec, voidf};
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
TEST(AsyncJobTest, ThrowException) {
		FiberExecutor exec{};
		Job j{exec, []() { throw std::runtime_error("hello"); }};
		j();
		ASSERT_ANY_THROW(exec.run_all());
}

TEST(AsyncJobTest, IntervalJob) {
		FifoExecutor<1> exec{};
		exec.start();
		upp::timer::OneShot timer{};
		std::atomic<int> i = 0;
		IntervalJob interval_job{exec, timer, 10ms, [&]() { ++i; }};

		std::this_thread::sleep_for(100ms);
		ASSERT_GT(i, 7);
}

/// Test that all jobs are executed sequentially
TEST(AsyncJobTest, Synchronization) {
		FifoExecutor<1> exec{};
		upp::timer::OneShot timer{};
		std::mutex mut{};
		int i = 0;
		IntervalJob ijob{exec, timer, 10ms, [&]() {
								 EXPECT_TRUE(mut.try_lock());
								 ++i;
								 mut.unlock();
						 }};

		Job j{exec, [&]() {
					  EXPECT_TRUE(mut.try_lock());
					  ++i;
					  mut.unlock();
			  }};

		exec.start();
		std::array<std::thread, 20> threads{};
		for (auto& t : threads) {
				t = std::thread([&]() {
						for (size_t i = 0; i < 100; ++i) { j(); }
				});
		}

		std::this_thread::sleep_for(100ms);

		for (auto& t : threads) { t.join(); }
}