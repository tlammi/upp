#pragma once

#include <coroutine>
#include <utility>

namespace upp::async {

template <class T = void>
class UniqueHandle {
    std::coroutine_handle<T> m_hndl{};

 public:
    using promise_type = T;
    constexpr UniqueHandle() noexcept = default;
    constexpr explicit UniqueHandle(std::coroutine_handle<T> h) noexcept
        : m_hndl(h) {}

    UniqueHandle(const UniqueHandle&) = delete;
    UniqueHandle& operator=(const UniqueHandle&) = delete;

    constexpr UniqueHandle(UniqueHandle&& other) noexcept
        : m_hndl(std::exchange(other.m_hndl, {})) {}

    constexpr UniqueHandle& operator=(UniqueHandle&& other) {
        auto tmp = UniqueHandle(std::move(other));
        std::swap(m_hndl, tmp.m_hndl);
        return *this;
    }

    constexpr ~UniqueHandle() {
        if (m_hndl) m_hndl.destroy();
    }

    constexpr explicit operator bool() const noexcept { return m_hndl; }
    constexpr bool done() const noexcept { return m_hndl.done(); }

    constexpr void resume() { m_hndl.resume(); }
    constexpr void operator()() { m_hndl(); }

    std::coroutine_handle<T> get() const noexcept { return m_hndl; }

    auto& promise() const noexcept
        requires(!std::same_as<T, void>)
    {
        return m_hndl.promise();
    }

    void* address() const noexcept { return m_hndl.address(); }

    std::coroutine_handle<T> release() noexcept {
        return std::exchange(m_hndl, {});
    }

    void reset(std::coroutine_handle<T> handle = {}) noexcept {
        UniqueHandle(std::exchange(m_hndl, handle));
    }

    operator UniqueHandle<void>() && requires(!std::same_as<T, void>) {
        return UniqueHandle<void>{
            std::coroutine_handle<>(std::exchange(m_hndl, {}))};
    }
};
}  // namespace upp::async
