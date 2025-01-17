#pragma once

#include <filesystem>
#include <upp/codec/detail/forward.hpp>
#include <upp/codec/detail/internal.hpp>
#include <upp/codec/packet.hpp>
#include <upp/frac.hpp>
#include <upp/uncopyable.hpp>

namespace upp::codec {
class demux;
class stream_meta_span;

class stream_meta {
    friend demux;
    friend stream_meta_span;
    AVStream* m_s{};

    constexpr explicit stream_meta(AVStream* stream) noexcept : m_s(stream) {}

 public:
    constexpr stream_meta() noexcept = default;

    constexpr explicit operator bool() const noexcept { return m_s != nullptr; }

    [[nodiscard]] bool video() const noexcept;
    [[nodiscard]] bool audio() const noexcept;

    [[nodiscard]] frac<int> fps() const noexcept;

    /**
     * \brief Use heuristics to deduce whether the stream is a still image
     * */
    [[nodiscard]] bool sill_image() const noexcept;

    AVStream* handle(detail::internal_t) const noexcept { return m_s; }
};

class stream_meta_span {
    friend demux;

    std::span<AVStream*> m_streams;

    constexpr stream_meta_span(AVStream** streams, std::size_t count) noexcept
        : m_streams(streams, count) {}

 public:
    constexpr stream_meta operator[](std::size_t idx) const noexcept {
        return stream_meta(m_streams[idx]);
    }

    constexpr std::size_t size() const noexcept { return m_streams.size(); };
    constexpr std::size_t length() const noexcept { return m_streams.size(); };
};

class demux : public uncopyable {
    std::span<const std::byte> m_buf{};  // Buffer if demuxing from memory
    size_t m_buf_offset{};
    AVFormatContext* m_ctx;
    AVIOContext* m_io_ctx{};

    // TODO: Could be moved to .cpp and made plain functions
    static int buffer_read(void* userdata, uint8_t* buf, int buflen) noexcept;
    static int64_t buffer_seek(void* userdata, int64_t offset,
                               int origin) noexcept;

 public:
    demux();
    explicit demux(const std::filesystem::path& src);

    explicit demux(std::span<const std::byte> buf);

    constexpr demux(detail::internal_t, AVFormatContext* ctx) noexcept
        : m_ctx(ctx) {}

    demux(demux&& other) noexcept;
    demux& operator=(demux&& other) noexcept;

    ~demux();

    bool operator>>(packet& pkt);

    stream_meta_span stream_info() noexcept;

    /**
     * \brief Best effort guess
     * */
    std::size_t best_video_stream();
};
}  // namespace upp::codec
