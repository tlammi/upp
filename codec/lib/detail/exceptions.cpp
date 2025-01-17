#include <upp/codec/detail/exceptions.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
}

namespace upp::codec::detail {
namespace {
std::string av_error_string(int code) {
    auto buf = std::string(AV_ERROR_MAX_STRING_SIZE, '\0');
    av_make_error_string(buf.data(), buf.size(), code);
    return buf;
}
}  // namespace

exception::exception(int ec) : std::runtime_error(av_error_string(ec)) {}
exception::exception(const char* what) : std::runtime_error(what) {}
}  // namespace upp::codec::detail
