#include "upp/async.hpp"

#include <iostream>

namespace async = upp::async;

int g_integer{0};

async::FifoExecutor<1> exec;
async::Job increment{exec, [](int i) {
							 g_integer += i;
							 return g_integer;
					 }};
async::Job decrement{exec, [](int i) {
							 g_integer -= i;
							 return g_integer;
					 }};

async::Job multiply{exec, [](int i) {
							g_integer *= i;
							return g_integer;
					}};

async::Job get_val{exec, []() { return g_integer; }};

async::Job increment_and_decrement{exec, [](int i, int d) {
										   g_integer += i;
										   g_integer -= d;
								   }};
int main() {
		exec.start();

		increment(10);
		decrement(100);
		multiply(10);
		increment_and_decrement(1000, 10);
		auto fut = get_val();
		fut.wait();
		std::cerr << "g_integer is: " << fut.get() << '\n';
}