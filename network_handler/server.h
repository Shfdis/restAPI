#ifndef SERVER_H
#define SERVER_H

#include <atomic>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include "router.h"
#include <boost/beast/ssl.hpp>

namespace asio = boost::asio;
namespace ssl = asio::ssl;
namespace beast = boost::beast;
namespace http = beast::http;
using tcp = asio::ip::tcp;

class Server {
public:
  explicit Server(asio::io_context &context, const std::string& cert_file, const std::string& key_file);
  ~Server();

  void run();
  void stop();

  Router router_;

private:
  void accept_connections();
  void handle_request(std::shared_ptr<beast::ssl_stream<tcp::socket>> stream);

  asio::io_context &context_;
  ssl::context ssl_ctx_;
  tcp::acceptor acceptor_;
  asio::signal_set signals_;
  std::atomic<bool> running_{false};
};

#endif