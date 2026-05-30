#pragma once

#include <upp/immovable.hpp>
#include <upp/rest/endpoint.hpp>
#include <upp/uncopyable.hpp>

namespace upp::rest {

template <class Ctx = void, class... Services>
class server {
 public:
};

template <class... Services>
class server<void, Services...> {
 public:
    server(std::string_view prefix, Services... services);
};

}  // namespace upp::rest
