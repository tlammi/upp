#include <gtest/gtest.h>

#include <upp/functional.hpp>

template <class T>
using Fn = upp::Function<T>;

template <class T>
constexpr auto mk_fn(T&& t) {
  return upp::Function(std::forward<T>(t));
}

TEST(Func, CtorDefault) {
  Fn<void()> f{};
  ASSERT_FALSE(f);
  ASSERT_EQ(f.callback(), nullptr);
  ASSERT_EQ(f.callback2(), nullptr);
}

TEST(Func, CtorLambda) {
  auto f = Fn<void()>([] {});
  ASSERT_TRUE(f);
}

TEST(Func, CtorDeduce) {
  auto f = upp::Function([] -> int { return 1; });
  static_assert(std::same_as<decltype(f), Fn<int()>>);
}

TEST(Func, NoRetVal) {
  int val = 0;
  auto f = upp::Function([&] { ++val; });
  ASSERT_EQ(val, 0);
  f();
  ASSERT_EQ(val, 1);
}

TEST(Func, RetInt) {
  auto f = mk_fn([] { return 1; });
  ASSERT_EQ(f(), 1);
  ASSERT_EQ(f(), 1);
}

TEST(Func, Add) {
  auto f = mk_fn([](int a, int b) { return a + b; });
  ASSERT_EQ(f(0, 0), 0);
  ASSERT_EQ(f(1, 2), 3);
}

TEST(Func, MoveOnly) {
  auto ptr = std::make_unique<int>(1);
  auto f = mk_fn([](std::unique_ptr<int> i) { return *i; });
  ASSERT_EQ(f(std::move(ptr)), 1);
}

TEST(Func, Callback) {
  auto f = mk_fn([](int i) { return 2 * i; });
  auto cb = f.callback();
  void* userdata = f.userdata();
  ASSERT_EQ(cb(userdata, 3), 6);
}

TEST(Func, Callback2) {
  auto f = mk_fn([](int i) { return 2 * i; });
  auto cb = f.callback2();
  void* userdata = f.userdata();
  ASSERT_EQ(cb(3, userdata), 6);
}

TEST(Func, Constexpr) {
  static constexpr auto res = mk_fn([](int i) { return i * 2; })(3);
  ASSERT_EQ(res, 6);
}

