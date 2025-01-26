#pragma once

#include <upp/bits/str/c_string.hpp>
#include <upp/bits/str/static_string.hpp>
#include <utility>

namespace upp {

class ConstString {
    CString m_str{};
    bool m_dynamic{false};

 public:
    constexpr ConstString() noexcept = default;
    constexpr ConstString(StaticString str) : m_str(str) {}
    constexpr ConstString(const std::string& str)
        : m_str(upp::strdup(str.c_str(), str.size())), m_dynamic(true) {}

    constexpr ConstString(const ConstString& other)
        : m_str(other.m_dynamic
                    ? upp::strdup(other.m_str.c_str(), other.m_str.size())
                    : other.m_str),
          m_dynamic(other.m_dynamic) {}

    constexpr ConstString& operator=(const ConstString& other) {
        auto tmp = ConstString(other);
        std::swap(tmp.m_dynamic, m_dynamic);
        std::swap(tmp.m_str, m_str);
        return *this;
    }

    constexpr ConstString(ConstString&& other) noexcept
        : m_str(other.m_str),
          m_dynamic(std::exchange(other.m_dynamic, false)) {}

    constexpr ConstString& operator=(ConstString&& other) noexcept {
        auto tmp = ConstString(std::move(other));
        std::swap(tmp.m_str, m_str);
        std::swap(tmp.m_dynamic, m_dynamic);
        return *this;
    }

    constexpr ~ConstString() {
        if (m_dynamic) delete[] m_str.c_str();
    }
};
}  // namespace upp
