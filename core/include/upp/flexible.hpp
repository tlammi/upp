#pragma once

#include <cmath>
#include <cstddef>

namespace upp {

namespace flexible_detail {
template <class T, class Hdr>
struct layout {
    Hdr header;
    T value;
};
}  // namespace flexible_detail

template <class T, class Hdr>
struct alignas(flexible_detail::layout<T, Hdr>) flexible final {
 private:
    using layout = flexible_detail::layout<T, Hdr>;
    static constexpr auto data_offset = offsetof(layout, value);

    static constexpr auto required_bytes(size_t elements) noexcept {
        return data_offset + elements * sizeof(T);
    }

 public:
    template <class Alloc>
    static flexible* create(size_t elements, Alloc& alloc) {
        static_assert(sizeof(typename Alloc::value_type) == 1,
                      "flexible requires allocator that allocates bytes");
        auto* mem = reinterpret_cast<flexible*>(
            alloc.allocate(required_bytes(elements)));
        std::construct_at(mem);
        return mem;
    }

    static flexible* create(size_t elements) {
        auto* mem =
            static_cast<flexible*>(::operator new(required_bytes(elements)));
        std::construct_at(mem);
        return mem;
    }

    static void destroy(flexible* flex) {
        std::destroy_at(flex);
        ::operator delete(flex);
    }

    template <class Alloc>
    static void destroy(flexible* flex, Alloc& alloc, size_t elements) {
        using alloc_value_type = typename Alloc::value_type;
        std::destroy_at(flex);
        alloc.deallocate(reinterpret_cast<alloc_value_type*>(flex),
                         required_bytes(elements));
    }

    Hdr header{};

    T* data() noexcept {
        auto addr = reinterpret_cast<std::ptrdiff_t>(this);
        addr += data_offset;
        return reinterpret_cast<T*>(addr);
    }

    const T* data() const noexcept {
        auto addr = reinterpret_cast<std::ptrdiff_t>(this);
        addr += data_offset;
        return reinterpret_cast<const T*>(addr);
    }
};

}  // namespace upp
