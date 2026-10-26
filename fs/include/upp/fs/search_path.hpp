#pragma once

#include <filesystem>
#include <string_view>

namespace upp::fs {

std::filesystem::path search_path(std::string_view what, std::string_view path);
std::filesystem::path search_path(std::string_view what);
}  // namespace upp::fs
