#include <upp/codec/frame.hpp>
#include <upp/exceptions.hpp>
#include <utility>

extern "C" {
#include <libavcodec/avcodec.h>
}
namespace upp::codec {
namespace {

AVFrame* dup_frame(const AVFrame* orig) {
    AVFrame* out = av_frame_alloc();
    UPP_CHECK(out, std::bad_alloc());
    int res = av_frame_ref(out, orig);
    UPP_CHECK(!res, std::bad_alloc());
    return out;
}

}  // namespace
frame::frame() : m_handle(av_frame_alloc()) {
    UPP_CHECK(m_handle, std::bad_alloc());
}

frame::frame(const frame& other) : m_handle(dup_frame(other.m_handle)) {}

frame& frame::operator=(const frame& other) {
    if (m_handle) av_frame_unref(m_handle);
    int res = av_frame_ref(m_handle, other.m_handle);
    UPP_CHECK(!res, std::bad_alloc());
    return *this;
}

frame::frame(frame&& other) noexcept
    : m_handle(std::exchange(other.m_handle, nullptr)) {}

frame& frame::operator=(frame&& other) noexcept {
    std::destroy_at(this);
    std::construct_at(this, std::move(other));
    return *this;
}

frame::~frame() {
    if (m_handle) {
        av_frame_unref(m_handle);
        av_frame_free(&m_handle);
    }
}

}  // namespace upp::codec
