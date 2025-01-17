#pragma once

namespace upp {

template <class T>
struct prototype_of : prototype_of<decltype(&T::operator())> {};

template <class R, class... Ps>
struct prototype_of<R(Ps...)> {
  using type = R(Ps...);
};

template <class R, class... Ps>
struct prototype_of<R (*)(Ps...)> {
  using type = R(Ps...);
};

template <class R, class C, class... Ps>
struct prototype_of<R (C::*)(Ps...)> {
  using type = R(Ps...);
};

template <class R, class C, class... Ps>
struct prototype_of<R (C::*)(Ps...) const> {
  using type = R(Ps...);
};

template <class T>
using prototype_of_t = typename prototype_of<T>::type;

}  // namespace upp
