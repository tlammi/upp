#pragma once

#include <memory>
#include <span>
#include <upp/codec/detail/forward.hpp>
#include <upp/codec/detail/internal.hpp>
#include <upp/uncopyable.hpp>
#include <utility>

namespace upp::codec {

class packet : public uncopyable {
    AVPacket* m_pkt;

 public:
    packet();

    packet(detail::internal_t, AVPacket* pkt) noexcept : m_pkt(pkt) {}

    packet(packet&& other) noexcept
        : m_pkt(std::exchange(other.m_pkt, nullptr)) {}

    packet& operator=(packet&& other) noexcept {
        std::destroy_at(this);
        std::construct_at(this, std::move(other));
        return *this;
    }

    ~packet();

    AVPacket* handle(detail::internal_t) const noexcept { return m_pkt; }

    std::span<const std::byte> data() const noexcept;

    std::size_t size() const noexcept;

    std::size_t stream() const noexcept;
};

}  // namespace upp::codec
