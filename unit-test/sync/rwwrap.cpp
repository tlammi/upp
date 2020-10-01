#include <gtest/gtest.h>

#include <thread>

#include "upp/sync.hpp"

TEST(SyncRwWrap, Ctor) {
		upp::sync::RwWrap<double> d{};
		upp::sync::RwWrap<int> i{100};
}

TEST(SyncRwWrap, ReadLocking) {
		upp::sync::RwWrap<int> i{0};
		auto locka = i.read_lock();
		auto lockb = i.read_lock();
		auto lockc = i.read_lock();
}

TEST(SyncRwWrap, Locking) {
		std::thread t{};
		upp::sync::RwWrap<int> i{0};
		{
				auto rlock = i.read_lock();
				ASSERT_EQ(*rlock, 0);
				t = std::thread([&]() { (*i.write_lock()) = 100; });
				ASSERT_EQ(*rlock, 0);
		}
		t.join();
		ASSERT_EQ(*i.read_lock(), 100);
}