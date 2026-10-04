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

namespace detail {

class fd_holder {
    native_handle m_handle{null_native_handle};

    void do_close() const noexcept;

 public:
    constexpr fd_holder() = default;
    constexpr explicit fd_holder(native_handle h) noexcept : m_handle(h) {}

    fd_holder(const fd_holder&) = delete;
    fd_holder& operator=(const fd_holder&) = delete;

    constexpr fd_holder(fd_holder&& other) noexcept
        : m_handle(std::exchange(other.m_handle, null_native_handle)) {}

    constexpr fd_holder& operator=(fd_holder&& other) noexcept {
        auto tmp = std::move(other);
        std::swap(m_handle, tmp.m_handle);
        return *this;
    }

    constexpr explicit operator bool() const noexcept {
        return m_handle != null_native_handle;
    }

    constexpr auto native() const noexcept { return m_handle; }
    constexpr auto release() noexcept {
        return std::exchange(m_handle, null_native_handle);
    }

 protected:
    constexpr ~fd_holder() {
        if (m_handle != null_native_handle) do_close();
    }
};

std::size_t write_impl(native_handle handle, std::span<const char> data);

template <class Self>
class writable_file_base {
 public:
    constexpr writable_file_base() noexcept = default;

    auto write(std::span<const char> data) {
        return write_impl(static_cast<Self*>(this)->native(), data);
    }

    auto write(std::span<std::byte> data) {
        return write(std::span<const char>(
            /*NOLINT*/ reinterpret_cast<const char*>(data.data()),
            data.size()));
    }

 protected:
    constexpr ~writable_file_base() = default;
};

}  // namespace detail

class file : public detail::fd_holder, public detail::writable_file_base<file> {
 public:
    using fd_holder::fd_holder;

    explicit file(const std::filesystem::path& path);

    file(const file&) = delete;
    file& operator=(const file&) = delete;

    constexpr file(file&& other) noexcept : fd_holder(std::move(other)) {}

    constexpr file& operator=(file&& other) noexcept {
        std::destroy_at(this);
        std::construct_at(this, std::move(other));
        return *this;
    }

    constexpr ~file() = default;

    std::size_t seek_begin(std::size_t offset = 0);
    std::size_t seek_current(std::ptrdiff_t offset = 0);
    std::size_t seek_end(std::ptrdiff_t offset = 0);

    std::size_t read(std::span<char> data);
    std::size_t read(std::span<std::byte> data);

    std::vector<std::byte> read_bin();
    std::string read_text();
};
}  // namespace upp::fs
