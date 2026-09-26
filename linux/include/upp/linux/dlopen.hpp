#pragma once

#include <upp/cstr.hpp>
#include <utility>

namespace upp::linux {

class dynamic_library {
    void* m_handle{};

    static void do_close(void* handle) noexcept;

 public:
    constexpr dynamic_library() noexcept = default;
    explicit dynamic_library(cstr path);
    dynamic_library(cstr path, int flags);

    dynamic_library(const dynamic_library&) = delete;
    dynamic_library& operator=(const dynamic_library&) = delete;

    constexpr dynamic_library(dynamic_library&& other) noexcept
        : m_handle(std::exchange(other.m_handle, nullptr)) {}

    constexpr dynamic_library& operator=(dynamic_library&& other) noexcept {
        std::destroy_at(this);
        std::construct_at(this, std::move(other));
        return *this;
    }

    constexpr ~dynamic_library() {
        if (m_handle) do_close(m_handle);
    }

    constexpr explicit operator bool() const noexcept {
        return m_handle != nullptr;
    }

    void* raw_symbol(cstr name) const;

    template<class T>
    T symbol(cstr name) const {
      return reinterpret_cast<T>(raw_symbol(name));
    }
};

}  // namespace upp::linux
