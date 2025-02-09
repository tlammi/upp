#pragma once

#include <boost/preprocessor.hpp>
#include <print>

namespace upp {
namespace assert_detail {

constexpr void do_assert(bool condition, std::string_view condition_string,
                         std::string_view file, std::size_t line,
                         std::string_view msg) {
    if (condition) return;
    if (msg.empty()) {
        std::println(stderr, "{}:{}: Assertion failed: [{}]", file, line,
                     condition_string);
    } else {
        std::println(stderr, "{}:{}: Assertion failed: [{}] '{}'", file, line,
                     condition_string, msg);
    }
    std::terminate();
}

}  // namespace assert_detail

// NOLINTNEXTLINE
#define UPP_DETAIL_ASSERT_2(condition, msg)                              \
    do {                                                                 \
        ::upp::assert_detail::do_assert(condition, #condition, __FILE__, \
                                        __LINE__, msg);                  \
    } while (0)

// NOLINTNEXTLINE
#define UPP_DETAIL_ASSERT_1(condition)                                   \
    do {                                                                 \
        ::upp::assert_detail::do_assert(condition, #condition, __FILE__, \
                                        __LINE__, "");                   \
    } while (false)

#if defined(NDEBUG)
#define UPP_ASSERT(condition, msg) \k
do {
} while (false);
#else
// NOLINTNEXTLINE
#define UPP_ASSERT(...) \
    BOOST_PP_OVERLOAD(UPP_DETAIL_ASSERT_, __VA_ARGS__)(__VA_ARGS__)
#endif
}  // namespace upp
