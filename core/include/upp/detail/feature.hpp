/**
 * \file
 *
 * Feature test stuff
 * */
#pragma once

#if __has_include(<meta>)
#include <meta>
#if __cpp_lib_reflection && __cpp_lib_reflection
#define UPP_HAVE_REFLECTION true
#endif
#endif

#ifndef UPP_HAVE_REFLECTION
#define UPP_HAVE_REFLECTION false
#endif

namespace upp::detail {
constexpr bool have_reflection = UPP_HAVE_REFLECTION;
}
