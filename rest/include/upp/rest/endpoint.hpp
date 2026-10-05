#pragma once

#include <boost/asio.hpp>
#include <filesystem>
#include <memory>

namespace upp::rest {

class endpoint {
 public:
    virtual ~endpoint() = default;
};

std::unique_ptr<endpoint> unix_socket_endpoint(
    boost::asio::io_context& io, const std::filesystem::path& path);

}  // namespace upp::rest
