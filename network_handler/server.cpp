#include "server.h"

Server::Server(asio::io_context &context, const std::string &cert_file,
               const std::string &key_file)
    : context_(context),
      ssl_ctx_(ssl::context::tlsv12),
      acceptor_(context_),
      signals_(context_, SIGINT, SIGTERM),
      running_(false)
{
    ssl_ctx_.set_options(
        ssl::context::default_workarounds |
        ssl::context::no_sslv2 |
        ssl::context::no_sslv3 |
        ssl::context::single_dh_use);

    ssl_ctx_.use_certificate_chain_file(cert_file);
    ssl_ctx_.use_private_key_file(key_file, ssl::context::pem);

    boost::system::error_code ec;
    acceptor_.open(tcp::v4(), ec);
    if (ec) throw std::runtime_error("Open failed: " + ec.message());

    acceptor_.set_option(tcp::acceptor::reuse_address(true), ec);
    if (ec) throw std::runtime_error("Set_option failed: " + ec.message());

    acceptor_.bind({tcp::v4(), 8443}, ec);
    if (ec) throw std::runtime_error("Bind failed: " + ec.message());

    acceptor_.listen(asio::socket_base::max_listen_connections, ec);
    if (ec) throw std::runtime_error("Listen failed: " + ec.message());

    signals_.async_wait([this](auto, auto) { this->stop(); });
}

Server::~Server() {
    stop();
}

void Server::run() {
    if (running_.exchange(true)) return;
    accept_connections();
}

void Server::stop() {
    if (!running_.exchange(false)) return;
    boost::system::error_code ec;
    acceptor_.close(ec);
    context_.stop();
}

void Server::accept_connections() {
    if (!running_.load()) return;

    auto socket = std::make_shared<tcp::socket>(context_);
    acceptor_.async_accept(*socket, [this, socket](beast::error_code ec) {
        if (ec || !running_.load()) {
            if (ec && ec != asio::error::operation_aborted)
                std::cerr << "Accept error: " << ec.message() << '\n';
            if (running_.load()) accept_connections();
            return;
        }

        auto ssl_stream = std::make_shared<beast::ssl_stream<tcp::socket>>(std::move(*socket), ssl_ctx_);
        ssl_stream->async_handshake(ssl::stream_base::server,
            [this, ssl_stream](beast::error_code ec) {
                if (ec) {
                    std::cerr << "Handshake error: " << ec.message() << '\n';
                } else {
                    handle_request(ssl_stream);
                }
                if (running_.load()) accept_connections();
            });
    });
}

void Server::handle_request(std::shared_ptr<beast::ssl_stream<tcp::socket>> stream) {
    auto buffer = std::make_shared<beast::flat_buffer>();
    auto req = std::make_shared<http::request<http::string_body>>();

    http::async_read(*stream, *buffer, *req,
        [this, stream, buffer, req](beast::error_code ec, std::size_t) {
            // Ignore expected truncation/EOF errors
            if (ec && ec != asio::error::eof && 
                ec != beast::errc::not_connected && 
                ec != asio::ssl::error::stream_truncated) {
                std::cerr << "Read error: " << ec.message() << '\n';
                return;
            }
            if (ec) {
                // Clean shutdown by client
                return;
            }

            auto res = std::make_shared<http::response<http::string_body>>();
            router_.handle_request(*req, *res);
            res->version(req->version());
            res->keep_alive(req->keep_alive());
            res->prepare_payload();

            http::async_write(*stream, *res,
                [this, stream, buffer, req, res](beast::error_code ec, std::size_t) {
                    if (ec) {
                        std::cerr << "Write error: " << ec.message() << '\n';
                        return;
                    }

                    if (res->keep_alive()) {
                        buffer->consume(buffer->size());
                        handle_request(stream);
                    } else {
                        beast::error_code shutdown_ec;
                        stream->shutdown(shutdown_ec);
                    }
                });
        });
}
