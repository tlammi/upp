/**
 * \file
 * */
#pragma once

#include <cassert>
#include <string>
#include <string_view>

namespace upp {

/**
 * \brief String view type with an additional guarantee for null termination
 * */
class cstr : private std::string_view {
    using base = std::string_view;

 public:
    constexpr cstr() noexcept : base("") {}
    constexpr cstr(const char* s) noexcept : base(s) {}
    template <size_t N>
    constexpr cstr(const char (&s)[N] /*NOLINT*/) noexcept : base(s) {}

    /**
     * \brief Create a cstr with a know length
     *
     * The behavior is undefined if the s[len] != '\0'.
     *
     * \param s String
     * \param len String length without null terminator
     * */
    constexpr cstr(const char* s, size_t len) noexcept : base(s, len) {
        assert(s[len] == '\0');  // NOLINT
    }

    using base::begin;
    using base::cbegin;
    using base::cend;
    using base::crbegin;
    using base::crend;
    using base::data;
    using base::end;
    using base::ends_with;
    using base::length;
    using base::rbegin;
    using base::rend;
    using base::size;
    using base::starts_with;

    using base::remove_prefix;

    const char* c_str() const noexcept { return data(); }

    constexpr explicit operator std::string() {
        auto view = static_cast<std::string_view>(*this);
        return std::string(view);
    }
};
}  // namespace upp
