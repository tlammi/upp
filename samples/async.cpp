#include "upp/async.hpp"

#include <iostream>

namespace async = upp::async;

int g_integer{0};

async::FifoExecutor<1> exec;
async::Job increment{exec, [](int i) { g_integer += i; }};
async::Job decrement{exec, [](int i) { g_integer -= i; }};

async::Job multiply{exec, [](int i) { g_integer *= i; }};

int main() {
		exec.start();

		increment(10);
		decrement(100);
		multiply(10);
		std::cerr << "g_integer is: " << g_integer << '\n';
}