#include <cassert>
#include <cstring>
#include <upp/cleanup.hpp>
#include <upp/codec/demux.hpp>
#include <upp/codec/detail/exceptions.hpp>
#include <upp/exceptions.hpp>

#include "logger.hpp"

extern "C" {
#include <libavformat/avformat.h>
}

namespace upp::codec {
namespace {
constexpr size_t AV_IO_BUFFER_SIZE = 8 * 1024;
}
int demux::buffer_read(void* userdata, uint8_t* buf, int buflen) noexcept {
    auto* self = static_cast<demux*>(userdata);
    assert(self->m_buf_offset <= self->m_buf.size());
    TRACE("buffer_read: size: {}, offset: {}", self->m_buf.size(),
          self->m_buf_offset);
    size_t left_in_buf = self->m_buf.size() - self->m_buf_offset;
    if (!left_in_buf) {
        TRACE("end-of-file");
        return AVERROR_EOF;
    }
    auto count = std::min(buflen, static_cast<int>(left_in_buf));
    std::memcpy(buf, &self->m_buf[self->m_buf_offset], count);
    self->m_buf_offset += count;
    TRACE("read {} bytes", count);
    return count;
}

int64_t demux::buffer_seek(void* userdata, int64_t offset,
                           int origin) noexcept {
    auto* self = static_cast<demux*>(userdata);
    TRACE("seek offset: current_offset={}, new_offset={}, origin={}",
          self->m_buf_offset, offset, origin);
    if (origin & AVSEEK_SIZE) {
        return static_cast<int64_t>(self->m_buf.size());
    }
    switch (origin) {
        case SEEK_SET: {  // 0
            TRACE("SEEK_SET");
            if (offset < 0) return -1;
            if (static_cast<size_t>(offset) > self->m_buf.size()) return -1;
            self->m_buf_offset = offset;
            return 0;
        }
        case SEEK_CUR: {  // 1
            TRACE("SEEK_CUR");
            auto new_idx = static_cast<int64_t>(self->m_buf_offset) + offset;
            if (new_idx < 0 ||
                static_cast<uint64_t>(new_idx) >= self->m_buf.size())
                return -1;
            self->m_buf_offset += offset;
            return 0;
        }
        case SEEK_END: {  // 2
            TRACE("SEEK_END");
            if (offset > 0) return -1;
            if (-offset >= static_cast<int64_t>(self->m_buf.size())) return -1;
            self->m_buf_offset = self->m_buf.size() + offset;
            TRACE("new offset: {}", self->m_buf_offset);
            return 0;
        }
        default: return -1;
    }
}

demux::demux() : m_ctx(avformat_alloc_context()) {
    UPP_CHECK(m_ctx, std::bad_alloc());
}
demux::demux(const std::filesystem::path& src) : demux() {
    // some one made a nice design decision and avformat_free_context() frees
    // the passed context on failure :)
    auto clean = upp::cleanup_throw([&] { m_ctx = nullptr; });
    auto res =
        avformat_open_input(&m_ctx, src.native().c_str(), nullptr, nullptr);
    UPP_CHECK(!res, detail::exception(res));
}
demux::demux(std::span<const std::byte> buf) : demux() {
    m_buf = buf;
    void* iobuf = av_malloc(AV_IO_BUFFER_SIZE + AV_INPUT_BUFFER_PADDING_SIZE);
    if (!iobuf) throw std::bad_alloc();
    auto clean_iobuf = cleanup_throw([&] { av_free(iobuf); });

    m_io_ctx = avio_alloc_context(
        static_cast<std::uint8_t*>(iobuf), AV_IO_BUFFER_SIZE, 0, this,
        &demux::buffer_read, nullptr, &demux::buffer_seek);
    if (!m_io_ctx) throw std::bad_alloc();
    m_ctx->pb = m_io_ctx;
    auto clean = cleanup_throw([&] { m_ctx = nullptr; });
    auto res = avformat_open_input(&m_ctx, "dummy", nullptr, nullptr);
    UPP_CHECK(!res, detail::exception(res));
}

demux::demux(demux&& other) noexcept
    : m_ctx(std::exchange(other.m_ctx, nullptr)) {}

demux& demux::operator=(demux&& other) noexcept {
    std::destroy_at(this);
    std::construct_at(this, std::move(other));
    return *this;
}

demux::~demux() {
    if (m_ctx) {
        avformat_close_input(&m_ctx);
        avformat_free_context(m_ctx);
    }
    if (m_io_ctx) {
        av_free(m_io_ctx->buffer);
        avio_context_free(&m_io_ctx);
    }
}

bool demux::operator>>(packet& pkt) {
    assert(m_ctx && "uninitialized context");
    auto* handle = pkt.handle(detail::internal);
    av_packet_unref(handle);
    int res = av_read_frame(m_ctx, handle);
    if (res == 0) [[likely]]
        return true;
    if (res == AVERROR_EOF) [[likely]]
        return false;
    throw detail::exception(res);
}

stream_meta_span demux::stream_info() noexcept {
    assert(m_ctx && "uninitialized context");
    return {m_ctx->streams, m_ctx->nb_streams};
}

std::size_t demux::best_video_stream() {
    assert(m_ctx && "uninitialized context");
    int res =
        av_find_best_stream(m_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    UPP_CHECK(res >= 0, detail::exception(res));
    return res;
}

}  // namespace upp::codec
