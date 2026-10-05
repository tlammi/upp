#pragma once

#include <functional>
#include <span>
#include <string>
#include <upp/rest/request.hpp>

namespace upp::rest {

class response {
 public:
    struct simple_text {
        unsigned status;
        std::string text;
        std::string content_type = "text/plain";
    };
    explicit response(simple_text spec);

    struct simple_json {
        unsigned status;
        std::string json;
        std::string content_type = "application/json";
    };
    explicit response(simple_json spec);

    struct chunked_bin {
        unsigned status;
        std::function<std::size_t(std::span<std::byte>, request&)> streamer;
        std::string content_type = "application/octet-stream";
    };
    explicit response(chunked_bin spec);

    struct chunked_text {
        unsigned status;
        std::function<std::size_t(std::span<char>, request&)> streamer;
        std::string content_type = "text/plain";
    };
};

}  // namespace upp::rest
