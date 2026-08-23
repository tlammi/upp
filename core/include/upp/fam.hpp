/**
 * \file
 * \ingroup upp_core
 * */
#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <upp/cleanup.hpp>

/**
 * \brief Workarounds for working with FAM objects
 * \ingroup upp_core
 *
 * C++ does not support FAM like C. This namespace provides tools for working
 * around that limitation.
 *
 * Example use:
 * ```cpp
 *
 * template<class T>
 * struct block{
 *     size_t count{};
 *     size_t capacity{};
 *
 *     block(const block&) = delete;
 *     block& operator=(const block&) = delete;
 *
 *     block(block&&) = delete;
 *     block& operator=(block&&) = delete;
 *
 *     ~block() {
 *        for(std::size_t i = 0; i < count; ++i) std::destroy_at(&data()[i]);
 *     }
 *
 *     T* data() noexcept { return upp::fam::get_fam<T>(this); }
 *
 * private:
 *     friend void create_block(std::size_t elem_count);
 *     constexpr block() = default;
 * };
 * block<T>* create_block(std::size_t elem_count){
 *      auto* ptr = upp::fam::alloc<block<T>, T>(elem_count);
 *      new (ptr) block<T>();
 *      ptr->capacity = elem_count;
 *      return ptr;
 * }
 *
 * ```
 * */
namespace upp::fam {

/**
 * \brief FAM object's alinment
 * \ingroup upp_core
 *
 * Returns alignment of the whole object, i.e. max alignment of the header and
 * the data.
 * */
template <class Hdr, class T>
constexpr auto alignment() noexcept {
    return std::max(alignof(Hdr), alignof(T));
}

namespace detail {
// Separate implementation for assertions
template <class Hdr, class T>
constexpr std::size_t offset_impl() noexcept {
    auto out = sizeof(Hdr);
    auto over = out % alignof(T);
    if (over == 0) return out;
    return out + alignof(T) - over;
}

}  // namespace detail

/**
 * \brief Offset of the FAM from the start of the object
 * \ingroup upp_core
 *
 * Equivalent to sizeof(Hdr) + <padding> where padding is amount of bytes
 * required to match alignof(T).
 * */
template <class Hdr, class T>
constexpr std::size_t offset() noexcept {
    auto out = detail::offset_impl<Hdr, T>();
    assert(out >= sizeof(Hdr) && "offset too small");
    assert(out % alignof(T) == 0 && "offset alignment mismatch");
    [[maybe_unused]] auto padding = out - sizeof(Hdr);
    assert(padding < alignof(T) && "offset too large");
    return out;
}

/**
 * \brief Total required bytes for an object
 * \ingroup upp_core
 *
 * Equivalent to sizeof(Hdr) + <padding> + sizeof(T) * elem_count.
 * */
template <class Hdr, class T>
constexpr std::size_t required_bytes(std::size_t elem_count) noexcept {
    return offset<Hdr, T>() + sizeof(T) * elem_count;
}

/**
 * \brief Get address of the first FAM member
 * \ingroup upp_core
 *
 * Given a FAM object of type Hdr, this returns address of the first member
 * appended to the object by other functions of this namespace.
 * */
template <class Hdr, class T>
constexpr T* get_fam(Hdr* h) noexcept {
    auto addr = reinterpret_cast<std::ptrdiff_t>(h);  // NOLINT
    addr += offset<Hdr, T>();
    return reinterpret_cast<T*>(addr);  // NOLINT
}

/**
 * \brief Allocate a FAM object
 * \ingroup upp_core
 *
 * Allocates correct amount of bytes using ::operator new to store Hdr and
 * elem_count of Ts. This only allocates the storage, constructors are not
 * called.
 * */
template <class Hdr, class T>
constexpr Hdr* alloc(std::size_t elem_count) {
    return static_cast<Hdr*>(
        ::operator new(required_bytes<Hdr, T>(elem_count),
                       std::align_val_t{alignment<Hdr, T>()}));
}

/**
 * \brief Deallocate a FAM object
 * \ingroup upp_core
 *
 * Deallocates bytes previously allocated with alloc(). Only deallocation is
 * performed, no destructor is called.
 * */
template <class Hdr, class T>
constexpr void dealloc(Hdr* hdr) {
    ::operator delete(static_cast<void*>(hdr),
                      std::align_val_t{alignment<Hdr, T>()});
}

}  // namespace upp::fam
