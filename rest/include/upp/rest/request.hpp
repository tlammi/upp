#pragma once

#include <string_view>
#include <upp/immovable.hpp>
#include <upp/maybe.hpp>
#include <upp/rest/method.hpp>
#include <upp/uncopyable.hpp>
#include <vector>

namespace upp::rest {

class request : public immovable, public uncopyable {
 public:
    rest::method method();
    bool chunked();
    std::string_view content_type();

    std::string text();
    std::vector<std::byte> data();

    maybe<std::size_t> read_some(std::span<char> buffer);
    maybe<std::size_t> read_some(std::span<std::byte> buffer);
};

}  // namespace upp::rest
