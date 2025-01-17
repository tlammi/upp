#include <fcntl.h>

#include <cstdlib>
#include <upp/cstr.hpp>
#include <upp/exceptions.hpp>
#include <upp/fs/tmpfile.hpp>
#include <upp/strlit.hpp>

using namespace upp::literals;

namespace upp::fs {
namespace {

const char* tmpdir_path() noexcept {
    const char* res = ::secure_getenv("TMPDIR");
    if (res) return res;
    return "/tmp/";
}

}  // namespace

file unnamed_tmpfile() {
    int fd =
        /*NOLINT*/ ::open(tmpdir_path(), O_TMPFILE | O_RDWR | O_EXCL,
                          S_IRUSR | S_IWUSR);
    if (fd < 0) throw_errno();
    return file{fd};
}

tmpdir::tmpdir() {
    static constexpr auto template_ = "XXXXXX"_lit;
    auto t = cstr(tmpdir_path());
    auto size = template_.size() + t.size();
    if (!t.ends_with('/')) ++size;
    auto buf = std::string();
    buf.reserve(size);
    buf.insert(buf.end(), t.begin(), t.end());
    if (!t.ends_with('/')) buf.push_back('/');
    buf.insert(buf.end(), template_.begin(), template_.end());
    auto* res = ::mkdtemp(buf.data());
    if (!res) throw_errno();
    m_path = std::filesystem::path(std::move(buf));
}
tmpdir::~tmpdir() {
    if (m_path.empty()) return;
    auto ec = std::error_code();
    std::filesystem::remove_all(m_path, ec);
    (void)ec;
}

}  // namespace upp::fs
