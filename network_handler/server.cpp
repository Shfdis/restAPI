#include "server.h"

Server::Server(asio::io_context &context, const std::string &cert_file,
               const std::string &key_file) : context_(context),
                                              ssl_ctx_(ssl::context::tlsv12),
                                              acceptor_(context_),
                                              signals_(context_, SIGINT, SIGTERM) {
    // Configure SSL context
    ssl_ctx_.set_options(
        ssl::context::default_workarounds |
        ssl::context::no_sslv2 |
        ssl::context::no_sslv3 |
        ssl::context::single_dh_use);

    // Load certificate files
    ssl_ctx_.use_certificate_chain_file(cert_file);
    ssl_ctx_.use_private_key_file(key_file, ssl::context::pem);

    // Configure acceptor
    boost::system::error_code ec;

    // 1. Open first
    acceptor_.open(tcp::v4(), ec);
    if (ec) {
        throw std::runtime_error("Open failed: " + ec.message());
    }

    // 2. Set reuse BEFORE bind
    acceptor_.set_option(tcp::acceptor::reuse_address(true), ec);
    if (ec) {
        throw std::runtime_error("Set option failed: " + ec.message());
    }

    // 3. Bind with retries
    int retries = 3;
    while (retries-- > 0) {
        acceptor_.bind({tcp::v4(), 8443}, ec);
        if (!ec) break;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    if (ec) throw std::runtime_error("Bind failed: " + ec.message());

    // 4. Listen
    acceptor_.listen(asio::socket_base::max_listen_connections, ec);
    if (ec) {
        throw std::runtime_error("Listen failed: " + ec.message());
    }

    // Safe signal handling
    signals_.async_wait([this](auto, auto) {
        std::cerr << "Shutting down...\n";
        this->stop();
    });
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
    if (ec) std::cerr << "Close error: " << ec.message() << "\n";
    context_.stop();
}

void Server::accept_connections() {
    if (!running_.load()) return;

    auto socket = std::make_shared<tcp::socket>(context_);
    acceptor_.async_accept(*socket, [this, socket](beast::error_code ec) {
        if (!running_.load()) {
            return;
        }
        if (ec) {
            if (ec != asio::error::operation_aborted) {
                std::cerr << "Accept error: " << ec.message() << "\n";
                accept_connections();
            }
            return;
        }

        // Create SSL stream and perform handshake
        auto stream = std::make_shared<beast::ssl_stream<tcp::socket> >(
            std::move(*socket), ssl_ctx_);

        stream->async_handshake(
            ssl::stream_base::server,
            [this, stream](beast::error_code ec) {
                if (ec) {
                    std::cerr << "SSL handshake error: " << ec.message() << "\n";
                    return;
                }
                handle_request(stream);
            });

        accept_connections();
    });
}

void Server::handle_request(std::shared_ptr<beast::ssl_stream<tcp::socket>> stream) {
    try {
        beast::flat_buffer buffer;
        http::request<http::string_body> req;

        // Read HTTPS request
        http::read(*stream, buffer, req);

        http::response<http::string_body> res;
        router_.handle_request(req, res);

        // Write HTTPS response
        http::write(*stream, res);

        // Graceful shutdown
        beast::error_code ec;
        stream->shutdown(ec);
        if (ec == beast::errc::not_connected || ec == asio::ssl::error::stream_truncated) {
            // These errors are expected when the client closes the connection
            ec = {};
        }
        if (ec) {
            throw beast::system_error{ec};
        }
    } catch (const std::exception &e) {
        std::cerr << "Request error: " << e.what() << "\n";
    }
}
