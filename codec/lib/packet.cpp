#include <cassert>
#include <upp/codec/packet.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
}

namespace upp::codec {

packet::packet() : m_pkt(av_packet_alloc()) {}

packet::~packet() {
    if (m_pkt) av_packet_free(&m_pkt);
}

std::span<const std::byte> packet::data() const noexcept {
    assert(m_pkt && "accessing empty packet");
    // NOLINTNEXTLINE
    return {reinterpret_cast<const std::byte*>(m_pkt->data),
            static_cast<std::size_t>(m_pkt->size)};
}

std::size_t packet::size() const noexcept {
    assert(m_pkt && "accessing empty packet");
    return static_cast<std::size_t>(m_pkt->size);
}

std::size_t packet::stream() const noexcept {
    assert(m_pkt && "accessing empty packet");
    assert(m_pkt->stream_index >= 0 && "Corrupted packet");
    return m_pkt->stream_index;
}

}  // namespace upp::codec
