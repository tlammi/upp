#pragma once

#include <filesystem>
#include <memory>
#include <span>
#include <upp/cast.hpp>
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

struct readable {};
struct writable {};
struct seekable {};

namespace detail {

class fd_holder {
    native_handle m_handle{null_native_handle};

    void do_close() const noexcept;

 public:
    constexpr fd_holder() = default;
    constexpr explicit fd_holder(native_handle h) noexcept : m_handle(h) {}
    explicit fd_holder(const std::filesystem::path& path);

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

template <class Self, class Trait>
class file_base;

std::size_t write_impl(native_handle handle, std::span<const char> data);

template <class Self>
class file_base<Self, writable> {
 public:
    constexpr file_base() noexcept = default;

    auto write(std::span<const char> data) {
        return write_impl(static_cast<Self*>(this)->native(), data);
    }

    auto write(std::span<std::byte> data) {
        return write(std::span<const char>(
            /*NOLINT*/ reinterpret_cast<const char*>(data.data()),
            data.size()));
    }

 protected:
    constexpr ~file_base() = default;
};

std::size_t read_impl(native_handle handle, std::span<char> data);

template <class Self>
class file_base<Self, readable> {
 public:
    constexpr file_base() noexcept = default;

    auto read(std::span<char> data) {
        return read_impl(static_cast<Self*>(this)->native(), data);
    }

    auto read(std::span<std::byte> data) { return read(span_cast<char>(data)); }

 protected:
    constexpr ~file_base() = default;
};

std::size_t seek_begin_impl(native_handle handle, std::size_t offset);
std::size_t seek_current_impl(native_handle handle, std::ptrdiff_t offset);
std::size_t seek_end_impl(native_handle handle, std::ptrdiff_t offset);

template <class Self>
class file_base<Self, seekable> {
 public:
    constexpr file_base() noexcept = default;

    auto seek_begin(std::size_t offset = 0) {
        return seek_begin_impl(static_cast<Self*>(this)->native(), offset);
    }
    auto seek_current(std::ptrdiff_t offset = 0) {
        return seek_current_impl(static_cast<Self*>(this)->native(), offset);
    }
    std::size_t seek_end(std::ptrdiff_t offset = 0) {
        return seek_end_impl(static_cast<Self*>(this)->native(), offset);
    }

 protected:
    constexpr ~file_base() = default;
};

}  // namespace detail

template <class... Traits>
class basic_file : public detail::fd_holder,
                   public detail::file_base<basic_file<Traits...>, Traits>... {
    using write_base = detail::file_base<basic_file<Traits...>, writable>;
    using seek_base = detail::file_base<basic_file<Traits...>, seekable>;
    using read_base = detail::file_base<basic_file<Traits...>, readable>;

    template <class Container, class CharType>
    Container read_impl() {
        auto curr = seek_base::seek_current();
        auto size = seek_base::seek_end() - curr;
        seek_base::seek_begin(curr);
        auto container = Container(size, CharType{});
        auto count = read_base::read(container);
        if (count != size) std::runtime_error("unexpected amount of data read");
        return container;
    }

 public:
    static constexpr bool is_writable = (std::same_as<Traits, writable> || ...);
    static constexpr bool is_readable = (std::same_as<Traits, readable> || ...);
    static constexpr bool is_seekable = (std::same_as<Traits, seekable> || ...);
    using detail::fd_holder::fd_holder;
    explicit basic_file(const std::filesystem::path& path)
        : detail::fd_holder(path) {}
    basic_file(const basic_file&) = delete;
    basic_file& operator=(const basic_file&) = delete;

    constexpr basic_file(basic_file&& other) noexcept
        : fd_holder(std::move(other)) {}

    constexpr basic_file& operator=(basic_file&& other) noexcept {
        fd_holder::operator=(std::move(other));
        return *this;
    }

    constexpr ~basic_file() = default;

    std::vector<std::byte> read_bin()
        requires(is_readable && is_seekable)
    {
        return read_impl<std::vector<std::byte>, std::byte>();
    }

    std::string read_text()
        requires(is_readable && is_seekable)
    {
        return read_impl<std::string, char>();
    }
};

using readable_file = basic_file<readable>;
using writable_file = basic_file<writable>;
using file = basic_file<writable, readable, seekable>;

}  // namespace upp::fs
