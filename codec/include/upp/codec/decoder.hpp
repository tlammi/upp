#pragma once

#include <upp/codec/demux.hpp>
#include <upp/codec/frame.hpp>
#include <upp/codec/hwaccel.hpp>
#include <upp/codec/packet.hpp>

namespace upp::codec {

class decoder {
    AVCodecContext* m_ctx{};
    // This is the target for decoding. The output frames reference the same
    // data. The reference cout is thread safe.
    frame m_internal{};

#ifndef NDEBUG
    // Used for verifying that all the packets come from a single stream
    std::size_t m_stream_idx{std::numeric_limits<std::size_t>::max()};
#endif

 public:
    struct decoder_opts {
        stream_meta stream{};
        codec::hwaccel hwaccel{};
    };

    decoder();
    explicit decoder(decoder_opts opts);

    decoder(const decoder&) = delete;
    decoder& operator=(const decoder&) = delete;

    decoder(decoder&& other) noexcept;
    decoder& operator=(decoder&& other) noexcept;

    ~decoder();

    /**
     * \brief Send data for decoding
     * */
    decoder& operator<<(const packet& pkt);

    bool operator>>(frame& frm);

    int width() const noexcept;
    int height() const noexcept;
};
}  // namespace upp::codec
