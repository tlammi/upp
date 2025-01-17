#pragma once

#include <upp/codec/detail/forward.hpp>
#include <upp/codec/detail/internal.hpp>
namespace upp::codec {

class frame {
    AVFrame* m_handle;

 public:
    frame();
    constexpr frame(detail::internal_t, AVFrame* handle) noexcept
        : m_handle(handle) {}

    frame(const frame& other);
    frame& operator=(const frame& other);

    frame(frame&& other) noexcept;
    frame& operator=(frame&& other) noexcept;

    ~frame();

    [[nodiscard]] AVFrame* handle(detail::internal_t) noexcept {
        return m_handle;
    }

    [[nodiscard]] const AVFrame* handle(detail::internal_t) const noexcept {
        return m_handle;
    }
};
}  // namespace upp::codec
