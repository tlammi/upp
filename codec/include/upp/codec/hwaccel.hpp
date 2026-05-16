#pragma once

#include <iterator>
#include <upp/codec/detail/forward.hpp>
#include <upp/codec/detail/internal.hpp>

namespace upp::codec {

class hwaccel_meta;

class hwaccel {
    AVBufferRef* m_ref{};

 public:
    constexpr hwaccel() noexcept = default;
    explicit hwaccel(hwaccel_meta meta);

    hwaccel(const hwaccel& other);
    hwaccel& operator=(const hwaccel& other);

    hwaccel(hwaccel&& other) noexcept;
    hwaccel& operator=(hwaccel&& other) noexcept;

    ~hwaccel();

    constexpr explicit operator bool() const noexcept {
        return m_ref != nullptr;
    }

    AVBufferRef* handle(detail::internal_t) const noexcept { return m_ref; }
};

class hwaccel_range {
 public:
    class iterator {
        friend hwaccel_range;

        unsigned m_type{};

        constexpr iterator(decltype(m_type) type) noexcept : m_type(type) {}

     public:
        using difference_type = std::ptrdiff_t;
        using value_type = hwaccel_meta;

        iterator& operator++() noexcept;
        iterator operator++(int) noexcept;

        value_type operator*() const noexcept;

        bool operator==(std::default_sentinel_t) const noexcept;
        bool operator!=(std::default_sentinel_t) const noexcept {
            return !(*this == std::default_sentinel);
        }
    };

    iterator begin() const noexcept;

    constexpr auto end() const noexcept { return std::default_sentinel; }
};

class hwaccel_meta {
    friend hwaccel_range::iterator;
    friend hwaccel;

    unsigned m_type;

    constexpr explicit hwaccel_meta(decltype(m_type) type) noexcept
        : m_type(type) {}

 public:
    const char* name() const noexcept;
};

constexpr hwaccel_range supported_hardware_accelerators() noexcept {
    return {};
}

hwaccel lookup_hardware_accelerator(const char* name);

}  // namespace upp::codec
