#pragma once

#include <filesystem>
#include <upp/fs/file.hpp>

namespace upp::fs {

/**
 * \brief Construct an unnamed temporary file
 *
 * The file is automatically deleted upon close.
 * */
file unnamed_tmpfile();

class tmpdir {
    std::filesystem::path m_path{};

 public:
    tmpdir();

    tmpdir(const tmpdir&) = delete;
    tmpdir& operator=(const tmpdir&) = delete;

    tmpdir(tmpdir&& other) = default;
    tmpdir& operator=(tmpdir&& other) = default;

    ~tmpdir();

    const auto& path() const noexcept { return m_path; }
};

}  // namespace upp::fs
