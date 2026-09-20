#pragma once

namespace upp {

template <class T, class... Ts>
struct first_of {
    using type = T;
};

template <class... Ts>
using first_of_t = typename first_of<Ts...>::type;

}  // namespace upp
