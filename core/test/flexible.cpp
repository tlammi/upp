#include <gtest/gtest.h>

#include <upp/flexible.hpp>

struct header {
    size_t capacity;
    size_t offset{};
    size_t count{};
};

template <class T>
using flexible = upp::flexible<T, header>;

template <class T>
void destroy(flexible<T>* f) {
    for (auto idx = 0u; idx < f->header.count; ++idx) {
        std::destroy_at(&f->data()[idx + f->header.offset]);
    }
    flexible<T>::destroy(f);
}
template <class T>
using flexible_ptr =
    std::unique_ptr<flexible<T>, decltype([](auto* ptr) { destroy(ptr); })>;

template <class T>
auto create(size_t elements) {
    auto* f = flexible<T>::create(elements);
    f->header = {
        .capacity = elements,
    };
    return flexible_ptr<T>(f);
}

template <class T, class Alloc>
struct alloc_deleter {
    Alloc alloc;

    void operator()(flexible<T>* flex) {
        flexible<T>::destroy(flex, alloc, flex->header.capacity);
    }
};
template <class T>
using flexible_alloc_ptr =
    std::unique_ptr<flexible<T>, alloc_deleter<T, std::allocator<std::byte>>>;

template <class T, class Alloc>
auto create(size_t elements, Alloc& alloc) {
    auto* f = flexible<T>::create(elements, alloc);
    f->header = {
        .capacity = elements,
    };
    return flexible_alloc_ptr<T>(f, alloc_deleter<T, Alloc>{alloc});
}

TEST(Allocate, Empty) {
    auto f = create<int>(0);
    ASSERT_EQ(f->header.capacity, 0);
}

TEST(Allocate, EmptyAllocator) {
    auto alloc = std::allocator<std::byte>();
    auto f = create<int>(0, alloc);
}

TEST(Allocate, Some) {
    static constexpr auto count = 100u;
    auto f = create<int>(count);
    ASSERT_EQ(f->header.capacity, count);
}

TEST(Allocate, SomeAllocator) {
    static constexpr auto count = 100u;
    auto alloc = std::allocator<std::byte>();
    auto f = create<int>(count, alloc);
}

TEST(Access, Some) {
    static constexpr auto count = 3;
    auto f = create<int>(count);
    for (auto i = 0; i < count; ++i) f->data()[i] = i;
    for (auto i = 0; i < count; ++i) ASSERT_EQ(f->data()[i], i);
}

TEST(Access, SomeAllocator) {
    auto alloc = std::allocator<std::byte>();
    static constexpr auto count = 3;
    auto f = create<int>(count, alloc);
    for (auto i = 0; i < count; ++i) f->data()[i] = i;
    for (auto i = 0; i < count; ++i) ASSERT_EQ(f->data()[i], i);
}
