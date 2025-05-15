//
// Created by shfdis on 5/14/25.
//

#ifndef ROUTER_H
#define ROUTER_H
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>

#include "path_trie.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class Router {
  using request = std::function<void(const http::request<http::string_body> &,
                                     http::response<http::string_body> &)>;
  PathTrie<request> routes_;
  static constexpr std::string possible_requests[4] = {"GET", "PUT", "POST",
                                                       "DELETE"};

 public:
  void add_path(const std::string &method, const std::string &path,
                request handler);

  void handle_request(const http::request<http::string_body> &req,
                      http::response<http::string_body> &res);
};

#endif  // ROUTER_H
