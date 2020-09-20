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