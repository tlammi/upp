#pragma once

#include <upp/logs.hpp>

namespace upp::rest {

const auto LOGGER = logs::context({.name = "upp.rest"});

#define UPP_CODEC_LOG(lvl, ...) UPP_LOG(lvl, ::upp::rest::LOGGER, __VA_ARGS__)

#define TRACE(...) UPP_CODEC_LOG(::upp::logs::level::trace, __VA_ARGS__)
#define DEBUG(...) UPP_CODEC_LOG(::upp::logs::level::debug, __VA_ARGS__)
#define INFO(...)  UPP_CODEC_LOG(::upp::logs::level::info, __VA_ARGS__)
#define NOTE(...)  UPP_CODEC_LOG(::upp::logs::level::note, __VA_ARGS__)
#define WARN(...)  UPP_CODEC_LOG(::upp::logs::level::warn, __VA_ARGS__)
#define ERR(...)   UPP_CODEC_LOG(::upp::logs::level::error, __VA_ARGS__)
#define FATAL(...) UPP_CODEC_LOG(::upp::logs::level::fatal, __VA_ARGS__)

}  // namespace upp::rest
