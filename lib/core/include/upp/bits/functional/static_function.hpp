#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <upp/concepts.hpp>
#include <upp/type_traits.hpp>
#include <upp/util.hpp>
#include <utility>

namespace upp {

template <class T, size_t S = sizeof(void*)>
class StaticFunction;

template <class T>
struct is_static_function : std::false_type {};

template <class T, size_t S>
struct is_static_function<StaticFunction<T, S>> : std::true_type {};

template <class T>
concept static_function = is_static_function<T>::value;

template <class T, class Proto, size_t S>
concept static_function_element =
    concepts::prototype<T, Proto> && sizeof(std::remove_cvref_t<T>) <= S &&
    !static_function<T> && std::is_trivially_copyable_v<T> &&
    std::is_trivially_destructible_v<T>;

template <size_t S, class R, class... Ps>
class StaticFunction<R(Ps...), S> {
    std::array<uint8_t, S> m_buf{};
    R (*m_invoke)(void*, Ps...){};

    void* data() noexcept { return static_cast<void*>(m_buf.data()); }
    const void* data() const noexcept {
        return static_cast<const void*>(m_buf.data());
    }

 public:
    constexpr StaticFunction() noexcept = default;

    // template <static_function_element<R(Ps...), S> T>
    template <static_function_element<R(Ps...), S> T>
    StaticFunction(T&& t)
        : m_invoke(+[](void* userdata, Ps... ps) {
              return (*static_cast<T*>(userdata))(std::forward<Ps>(ps)...);
          }) {
        upp::Defer defer{[&] noexcept { m_invoke = nullptr; }};
        std::construct_at(static_cast<T*>(data()), std::forward<T>(t));
        defer.cancel();
    }

    template <size_t S2>
    constexpr StaticFunction(StaticFunction<R(Ps...), S2> other)
        requires(S2 < S)
        : m_invoke(other.m_invoke) {
        std::copy(other.m_buf.begin(), other.m_buf.end(), m_buf.begin());
    }

    constexpr StaticFunction(const StaticFunction&) = default;
    constexpr StaticFunction& operator=(const StaticFunction&) = default;

    constexpr StaticFunction(StaticFunction&& other) noexcept = default;
    constexpr StaticFunction& operator=(StaticFunction&& other) noexcept =
        default;

    constexpr ~StaticFunction() = default;

    R operator()(Ps... ps) { return m_invoke(data(), std::forward<Ps>(ps)...); }

    void* userdata() noexcept {
        return reinterpret_cast<void*>(this);  // NOLINT
    }

    auto callback() noexcept -> R (*)(void*, Ps...) {
        return +[](void* userdata, Ps... ps) {
            auto* self = static_cast<StaticFunction*>(userdata);
            void* ptr = self->data();
            return self->m_invoke(ptr, std::forward<Ps>(ps)...);
        };
    }
    auto callback2() noexcept -> R (*)(Ps..., void*) {
        return +[](Ps... ps, void* userdata) {
            auto* self = static_cast<StaticFunction*>(userdata);
            void* ptr = self->data();
            return self->m_invoke(ptr, std::forward<Ps>(ps)...);
        };
    }

    constexpr explicit operator bool() const noexcept { return m_invoke; }
};

template <class T>
StaticFunction(T) -> StaticFunction<prototype_of_t<T>>;

}  // namespace upp
