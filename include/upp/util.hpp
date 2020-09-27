/* SPDX-License-Identifier: MIT */
/* Copyright @ 2020 Toni Lammi */
#pragma once

#include <string>
namespace upp {
namespace util {

template <typename To>
To convert(const std::string& in);

template <typename To>
To convert(const char* const in) {
    return convert<To>(std::string(in));
}
}  // namespace util

}  // namespace upp