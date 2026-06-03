#pragma once

#include <filesystem>
#include <span>
#include <upp/codec/frame.hpp>

namespace upp::codec {

/**
 * \brief Decode a single frame from an image
 *
 * */
frame decode_image(std::span<const std::byte> data);

/**
 * \brief Decode a single frame from an image file
 * */
frame decode_image(const std::filesystem::path& path);

}  // namespace upp::codec
