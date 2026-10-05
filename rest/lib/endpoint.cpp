#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <upp/rest/endpoint.hpp>

#include "logger.hpp"

namespace upp::rest {
namespace {
constexpr std::size_t buffer_size = 8192;
}

template <class Proto>
class connection : public std::enable_shared_from_this<connection<Proto>> {
    using self_ptr = std::shared_ptr<connection>;

    Proto::socket m_sock;
    boost::beast::flat_buffer m_buf{buffer_size};
    boost::beast::http::response<boost::beast::http::dynamic_body> m_resp{};
    boost::beast::http::request<boost::beast::http::dynamic_body> m_req{};

    explicit connection(Proto::socket sock) : m_sock(std::move(sock)) {}

    void start() {
        boost::beast::http::async_read(
            m_sock, m_buf, m_req,
            [self = this->shared_from_this()](boost::beast::error_code ec,
                                              std::size_t bytes_transferred) {
                if (ec) {
                    if (ec == boost::beast::errc::operation_canceled) {
                        TRACE("connection: Operation cancelled. Shutting down");
                        return;
                    }
                    WARN("connection: Error reading HTTP request: '{}'",
                         ec.what());
                    return;
                }
                self->process_request();
            });
    }

    void process_request() {
        m_resp.version(m_req.version());
        m_resp.keep_alive(false);
    }

 public:
    static connection create(Proto::socket sock) {
        return std::make_shared<connection>(std::move(sock));
    }
};

template <class Proto>
class endpoint_impl final : public endpoint {
    boost::asio::io_context* m_io;
    Proto::endpoint m_ep;
    Proto::acceptor m_acceptor{*m_io, m_ep};
    Proto::socket m_sock{*m_io};

    void accept_next() {
        m_acceptor.async_accept(m_sock, [&](boost::beast::error_code ec,
                                            Proto::socket sock) {
            if (ec) {
                if (ec == boost::beast::errc::operation_canceled) {
                    TRACE("endpoint: Operation cancelled. Shutting down");
                    return;
                }
                WARN("endpoint: Error accepting connection: '{}'", ec.what());
            } else {
            }
            accept_next();
        });
    }

 public:
    explicit endpoint_impl(boost::asio::io_context& io,
                           const std::filesystem::path& path)
        : m_io(&io), m_ep(path) {
        m_acceptor.async_accept(m_sock, [](boost::beast::error_code ec) {});
    }
};

std::unique_ptr<endpoint> unix_socket_endpoint(
    boost::asio::io_context& io, const std::filesystem::path& path) {
    return std::make_unique<endpoint_impl<boost::asio::local::stream_protocol>>(
        io, path);
}
}  // namespace upp::rest
