#pragma once

#include <concepts>
#include <upp/rest/request.hpp>
#include <upp/rest/response.hpp>

namespace upp::rest {

template <class T>
concept service = requires(T t, request req) {
    { t(req) } -> std::convertible_to<response>;
};

}  // namespace upp::rest
