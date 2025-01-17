#include <fcntl.h>
#include <unistd.h>

#include <cassert>
#include <upp/cast.hpp>
#include <upp/exceptions.hpp>
#include <upp/fs/file.hpp>

namespace upp::fs {

void file::do_close() noexcept {
    if (m_handle == null_native_handle) return;
    ::close(m_handle);
}
file::file(const std::filesystem::path& path) {
    // TODO: Should not create by default
    //
    // NOLINTNEXTLINE
    m_handle = ::open(path.c_str(), O_CREAT | O_RDWR);
    if (m_handle == null_native_handle) throw_errno();
}

std::size_t file::write(std::span<const char> data) {
    auto count = ::write(m_handle, data.data(), data.size());
    if (count < 0) throw_errno();
    return count;
}

std::size_t file::seek_begin(std::size_t offset) {
    static constexpr auto max =
        static_cast<std::size_t>(std::numeric_limits<off_t>::max());
    assert(offset <= max);
    auto res = ::lseek(m_handle, static_cast<off_t>(offset), SEEK_SET);
    if (res == static_cast<off_t>(-1)) throw_errno();
    return res;
}

std::size_t file::seek_end(std::ptrdiff_t offset) {
    auto res = ::lseek(m_handle, offset, SEEK_END);
    if (res == static_cast<off_t>(-1)) throw_errno();
    return res;
}

std::size_t file::seek_current(std::ptrdiff_t offset) {
    auto res = ::lseek(m_handle, offset, SEEK_CUR);
    if (res == static_cast<off_t>(-1)) throw_errno();
    return res;
}

std::size_t file::read(std::span<char> data) {
    auto count = ::read(m_handle, data.data(), data.size());
    if (count < 0) throw_errno();
    return count;
}
std::size_t file::read(std::span<std::byte> data) {
    auto spn = span_cast<char>(data);
    return read(spn);
}

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

}  // namespace upp::fs
