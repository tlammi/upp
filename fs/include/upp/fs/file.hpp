#pragma once

#include <filesystem>
#include <memory>
#include <span>
#include <utility>
#include <vector>

namespace upp::fs {

// TODO: Add aliases for other platforms if needed
using native_handle = int;

struct null_native_handle_t {
    friend constexpr bool operator==(null_native_handle_t,
                                     native_handle hndl) noexcept {
        return hndl < 0;
    }
    friend constexpr bool operator!=(null_native_handle_t null,
                                     native_handle hndl) noexcept {
        return !(null == hndl);
    }

    constexpr operator native_handle() const noexcept { return -1; }
};

constexpr null_native_handle_t null_native_handle{};

class file {
    native_handle m_handle{null_native_handle};

    void do_close() noexcept;

 public:
    constexpr file() noexcept = default;
    constexpr explicit file(native_handle h) noexcept : m_handle(h) {}

    explicit file(const std::filesystem::path& path);

    file(const file&) = delete;
    file& operator=(const file&) = delete;

    constexpr file(file&& other) noexcept
        : m_handle(std::exchange(other.m_handle, null_native_handle)) {}

    constexpr file& operator=(file&& other) noexcept {
        std::destroy_at(this);
        std::construct_at(this, std::move(other));
        return *this;
    }

    constexpr ~file() {
        if (m_handle != null_native_handle) do_close();
    }

    std::size_t write(std::span<const char> data);
    std::size_t seek_begin(std::size_t offset = 0);
    std::size_t seek_current(std::ptrdiff_t offset = 0);
    std::size_t seek_end(std::ptrdiff_t offset = 0);

    std::size_t read(std::span<char> data);
    std::size_t read(std::span<std::byte> data);

    std::vector<std::byte> read_bin();
    std::string read_text();

    constexpr auto native() const noexcept { return m_handle; }
    constexpr auto release() noexcept {
        return std::exchange(m_handle, null_native_handle);
    }
};
}  // namespace upp::fs
