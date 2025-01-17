#include <cassert>
#include <upp/codec/decoder.hpp>
#include <upp/codec/detail/exceptions.hpp>
#include <upp/exceptions.hpp>
#include <upp/logs.hpp>

#include "logger.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

namespace upp::codec {

decoder::decoder() : m_ctx(avcodec_alloc_context3(nullptr)) {}

decoder::decoder(decoder_opts opts) : decoder() {
    // TODO: Need to clean up stuff allocated by the constructor
    if (opts.hwaccel) {
        m_ctx->hw_device_ctx =
            av_buffer_ref(opts.hwaccel.handle(detail::internal));
    }
    if (opts.stream) {
        auto* handle = opts.stream.handle(detail::internal);
        int res = avcodec_parameters_to_context(m_ctx, handle->codecpar);
        UPP_CHECK(!res, detail::exception(res));
        m_ctx->pkt_timebase = handle->time_base;
        auto* codec = avcodec_find_decoder(m_ctx->codec_id);
        if (!codec) { throw std::runtime_error("codec not found"); }
        // TODO: Codec options?
        res = avcodec_open2(m_ctx, codec, nullptr);
        UPP_CHECK(!res, detail::exception(res));
    }
}

decoder::decoder(decoder&& other) noexcept
    : m_ctx(std::exchange(other.m_ctx, nullptr)),
      m_internal(std::move(other.m_internal))
#ifndef NDEBUG
      ,
      m_stream_idx(other.m_stream_idx)
#endif
{
}

decoder& decoder::operator=(decoder&& other) noexcept {
    std::destroy_at(this);
    std::construct_at(this, std::move(other));
    return *this;
}

decoder::~decoder() {
    if (m_ctx) avcodec_free_context(&m_ctx);
}

decoder& decoder::operator<<(const packet& pkt) {
#ifndef NDEBUG
    static constexpr auto max_idx = std::numeric_limits<std::size_t>::max();
    if (m_stream_idx == max_idx) m_stream_idx = pkt.stream();
    assert(m_stream_idx == pkt.stream());
#endif
    TRACE("decoder: enqueuing packet");
    auto res = avcodec_send_packet(m_ctx, pkt.handle(detail::internal));
    UPP_CHECK(!res, detail::exception(res));
    return *this;
}

bool decoder::operator>>(frame& frm) {
    av_frame_unref(frm.handle(detail::internal));
    TRACE("decoder: extracting frame");
    int res = avcodec_receive_frame(m_ctx, m_internal.handle(detail::internal));
    if (res == 0) {
        frm = m_internal;
        return true;
    }
    if (res == AVERROR(EAGAIN)) return false;
    throw detail::exception(res);
}
int decoder::width() const noexcept {
    assert(m_ctx);
    return m_ctx->width;
}

int decoder::height() const noexcept {
    assert(m_ctx);
    return m_ctx->height;
}

}  // namespace upp::codec
