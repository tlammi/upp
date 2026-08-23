#include <gtest/gtest.h>

#include <array>
#include <cstddef>
#include <upp/fam.hpp>

// NOLINTBEGIN(*magic-number*)
static_assert(upp::fam::offset<std::array<std::byte, 100>, char>() == 100);
static_assert(upp::fam::offset<std::array<std::byte, 3>, std::size_t>() ==
              alignof(std::size_t));
// NOLINTEND(*magic-number*)

template <class T>
struct header {
    std::size_t count{};

    constexpr header() noexcept = default;
    header(const header&) = delete;
    header& operator=(const header&) = delete;

    header(header&&) = delete;
    header& operator=(header&&) = delete;

    ~header() {
        auto* d = data();
        for (size_t i = 0; i < count; ++i) std::destroy_at(&d[i]);  // NOLINT
    }

    T* data() noexcept { return upp::fam::get_fam<header, T>(this); }
};

template <class T>
struct header_deleter {
    void operator()(header<T>* h) {
        std::destroy_at(h);
        upp::fam::dealloc<header<T>, T>(h);
    }
};

template <class T>
using header_ptr = std::unique_ptr<header<T>, header_deleter<T>>;

template <class T>
header_ptr<T> create_fam(std::size_t count) {
    auto h =
        header_ptr(upp::fam::alloc<header<T>, T>(count), header_deleter<T>());
    std::construct_at(h.get());
    for (size_t i = 0; i < count; ++i) {
        std::construct_at(&h->data()[i]);  // NOLINT
        ++h->count;
    }
    return h;
}

TEST(Simple, Access) {
    auto h = create_fam<int>(2);
    h->data()[0] = 1;
    h->data()[1] = 2;

    ASSERT_EQ(h->data()[0], 1);
    ASSERT_EQ(h->data()[1], 2);
}
