#include <fcntl.h>
#include <unistd.h>

#include <cassert>
#include <upp/cast.hpp>
#include <upp/exceptions.hpp>
#include <upp/fs/file.hpp>

namespace upp::fs {
namespace {

native_handle do_open(const std::filesystem::path& path) {
    // NOLINTNEXTLINE
    auto handle = ::open(path.c_str(), O_CREAT | O_RDWR);
    if (handle == null_native_handle) throw_errno();
    return handle;
}
}  // namespace

namespace detail {
void fd_holder::do_close() const noexcept {
    if (m_handle == null_native_handle) return;
    ::close(m_handle);
}

fd_holder::fd_holder(const std::filesystem::path& path)
    : m_handle(do_open(path)) {}

// NOLINTNEXTLINE(readability-make-member-function-const)
std::size_t write_impl(native_handle handle, std::span<const char> data) {
    auto count = ::write(handle, data.data(), data.size());
    if (count < 0) throw_errno();
    return count;
}

std::size_t read_impl(native_handle handle, std::span<char> data) {
    auto count = ::read(handle, data.data(), data.size());
    if (count < 0) throw_errno();
    return count;
}
std::size_t seek_begin_impl(native_handle handle, std::size_t offset) {
    static constexpr auto max =
        static_cast<std::size_t>(std::numeric_limits<off_t>::max());
    assert(offset <= max);
    auto res = ::lseek(handle, static_cast<off_t>(offset), SEEK_SET);
    if (res == static_cast<off_t>(-1)) throw_errno();
    return res;
}

std::size_t seek_current_impl(native_handle handle, std::ptrdiff_t offset) {
    auto res = ::lseek(handle, offset, SEEK_CUR);
    if (res == static_cast<off_t>(-1)) throw_errno();
    return res;
}
std::size_t seek_end_impl(native_handle handle, std::ptrdiff_t offset) {
    auto res = ::lseek(handle, offset, SEEK_END);
    if (res == static_cast<off_t>(-1)) throw_errno();
    return res;
}

}  // namespace detail

/*
file::file(const std::filesystem::path& path) : fd_holder(do_open(path)) {}

std::vector<std::byte> file::read_bin() {
    auto curr = seek_current();
    auto size = seek_end() - curr;
    seek_begin(curr);
    auto vec = std::vector<std::byte>(size, std::byte{});
    auto count = read(vec);
    if (count != size) std::runtime_error("unexpected amount of data read");
    return vec;
}

std::string file::read_text() {
    auto curr = seek_current();
    auto size = seek_end() - curr;
    seek_begin(curr);
    auto str = std::string(size, '\0');
    auto count = read(str);
    if (count != size) std::runtime_error("unexpected amount of data read");
    return str;
}
*/

}  // namespace upp::fs
