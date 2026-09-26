#pragma once

#include <upp/cstr.hpp>
#include <utility>
#include <filesystem>

namespace upp::linux {

/**
  * \brief Handle to a dynamically loaded library
  * */
class dynamic_library {
    void* m_handle{};

    static void do_close(void* handle) noexcept;

 public:
    /**
     * \brief Construct a null handle
     * */
    constexpr dynamic_library() noexcept = default;

    /**
     * \brief Open a dynamic library in path
     * */
    explicit dynamic_library(const char* path);
    explicit dynamic_library(cstr path): dynamic_library(path.c_str()){}
    explicit dynamic_library(const std::filesystem::path& path): dynamic_library(path.native().c_str()){}

    /**
     * \brief Open a dynamic library with flags
     * */
    dynamic_library(const char* path, int flags);
    dynamic_library(cstr path, int flags): dynamic_library(path.c_str(), flags){}
    dynamic_library(const std::filesystem::path& path, int flags): dynamic_library(path.native().c_str(), flags){}

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

    /**
     * \brief Operator bool
     *
     * True for handles pointing to a library, false for others
     * */
    constexpr explicit operator bool() const noexcept {
        return m_handle != nullptr;
    }

    /**
     * \brief Get an untyped handle to a symbol
     *
     * This can be simply casted to the actual type for use. The returned pointer
     * is never null. An exception is thrown if the symbol is not found.
     * */
    void* raw_symbol(cstr name) const;

    /**
     * \brief Get a typed handle to a symbol
     *
     * Utility for getting the symbol and casting it to the given type.
     * */
    template<class T>
    T symbol(cstr name) const {
      return reinterpret_cast<T>(raw_symbol(name));
    }
};

}  // namespace upp::linux
