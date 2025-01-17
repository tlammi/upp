#pragma once

#include <memory>

namespace upp::detail {

template <class Alloc, class = void>
class allocator_holder {
    Alloc m_alloc;

 protected:
    constexpr auto& get_allocator() noexcept { return m_alloc; }
    constexpr ~allocator_holder() = default;
};

template <class Alloc>
class allocator_holder<
    Alloc, std::enable_if_t<std::allocator_traits<Alloc>::is_always_equal()>> {
 protected:
    constexpr auto get_allocator() const
        noexcept(std::is_nothrow_constructible_v<Alloc>) {
        return Alloc();
    }
    constexpr ~allocator_holder() = default;
};
}  // namespace upp::detail
