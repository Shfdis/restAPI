//
// Created by shfdis on 5/14/25.
//

#include "router.h"

void Router::add_path(const std::string &method, const std::string &path,
                      request handler) {
  if (std::find(possible_requests, possible_requests + 4, method) ==
      possible_requests + 4) {
    throw std::invalid_argument("Invalid method");
  }
  routes_.Add(method + "/" + path, handler);
}

void Router::handle_request(const http::request<http::string_body> &req,
                            http::response<http::string_body> &res) {
  std::string key =
      std::string(req.method_string()) + "/" + std::string(req.target());
  auto it = routes_[key];
  if (it != nullptr) {
    (*it)(req, res);
  } else {
    res.result(http::status::not_found);
    res.body() = "Route not found";
  }
  res.prepare_payload();
}
