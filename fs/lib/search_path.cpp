#include <unistd.h>

#include <filesystem>
#include <ranges>
#include <stdexcept>
#include <upp/fs/search_path.hpp>

namespace upp::fs {
namespace {

auto to_directory_range(std::string_view path) noexcept {
    return std::ranges::subrange(std::filesystem::directory_iterator(path),
                                 std::filesystem::directory_iterator());
}
}  // namespace

std::filesystem::path search_path(std::string_view what,
                                  std::string_view path) {
    using namespace std::views;
    for (const auto& item :
         path | split(':') |
             transform([](auto in) { return std::string_view(in); }) |
             transform(to_directory_range) | join |
             transform(
                 [](const auto& entry) -> auto& { return entry.path(); }) |
             filter([&](const auto& path) { return path.filename() == what; }) |
             filter([&](const auto& path) {
                 return ::access(path.c_str(), X_OK) == 0;
             })) {
        return item;
    }
    return {};
}
std::filesystem::path search_path(std::string_view what) {
    const char* path = std::getenv("PATH");
    if (!path) [[unlikely]]
        throw std::runtime_error("PATH not set");
    return search_path(what, path);
}
}  // namespace upp::fs
