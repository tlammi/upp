#include <cassert>
#include <upp/cast.hpp>
#include <upp/codec/detail/exceptions.hpp>
#include <upp/codec/hwaccel.hpp>
#include <upp/exceptions.hpp>
#include <utility>

extern "C" {
#include <libavcodec/avcodec.h>
}

namespace upp::codec {
namespace {
AVBufferRef* alloc_hwdevice_ctx(AVHWDeviceType type) {
    assert(type != AV_HWDEVICE_TYPE_NONE);
    AVBufferRef* ctx{};
    auto res = av_hwdevice_ctx_create(&ctx, type, nullptr, nullptr, 0);
    UPP_CHECK(!res, detail::exception(res));
    return ctx;
}
}  // namespace

hwaccel::hwaccel(hwaccel_meta meta)
    : m_ref(alloc_hwdevice_ctx(underlying_cast<AVHWDeviceType>(meta.m_type))) {}

hwaccel::hwaccel(const hwaccel& other)
    : m_ref(other.m_ref ? av_buffer_ref(other.m_ref) : nullptr) {
    if (other.m_ref && !m_ref) [[unlikely]]
        throw std::bad_alloc();
}

hwaccel& hwaccel::operator=(const hwaccel& other) {
    std::destroy_at(this);
    std::construct_at(this, other);
    return *this;
}

hwaccel::hwaccel(hwaccel&& other) noexcept
    : m_ref(std::exchange(other.m_ref, nullptr)) {}

hwaccel& hwaccel::operator=(hwaccel&& other) noexcept {
    std::destroy_at(this);
    std::construct_at(this, std::move(other));
    return *this;
}

hwaccel::~hwaccel() {
    if (m_ref) av_buffer_unref(&m_ref);
}

auto hwaccel_range::begin() const noexcept -> iterator {
    return iterator(
        upp::underlying_cast(av_hwdevice_iterate_types(AV_HWDEVICE_TYPE_NONE)));
}

auto hwaccel_range::iterator::operator++() noexcept -> iterator& {
    m_type =
        av_hwdevice_iterate_types(upp::underlying_cast<AVHWDeviceType>(m_type));
    return *this;
}

auto hwaccel_range::iterator::operator++(int) noexcept -> iterator {
    auto tmp = *this;
    ++*this;
    return tmp;
}

auto hwaccel_range::iterator::operator*() const noexcept -> value_type {
    return value_type(m_type);
}

bool hwaccel_range::iterator::operator==(
    std::default_sentinel_t) const noexcept {
    return underlying_cast<AVHWDeviceType>(m_type) == AV_HWDEVICE_TYPE_NONE;
}

const char* hwaccel_meta::name() const noexcept {
    return av_hwdevice_get_type_name(underlying_cast<AVHWDeviceType>(m_type));
}

hwaccel lookup_hardware_accelerator(const char* name);
}  // namespace upp::codec
