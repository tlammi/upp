#pragma once

#include <filesystem>
#include <generator>

namespace upp::fs {

#if 0
enum class event_type : std::uint8_t {
    // File accessed (read or write)
    file_accessed = 0x01,
    // A file or directory was created
    created = 0x02,
    // a file or directory was deleted
    deleted = 0x04,
    // A file was modified (e.g. write())
    modified = 0x08,
    // a file or directory  was opened
    opened = 0x10,
};
#endif

struct event {
    std::filesystem::path target;
};

struct monitor_opts {
    bool recurse = false;
};

std::generator<event> monitor(std::filesystem::path root, monitor_opts = {});

}  // namespace upp::fs
