#include "network_handler/server.h"


int main() {
  asio::io_context context;
  std::string cert = "/home/shfdis/CLionProjects/restAPI/server.crt";
  std::string key = "/home/shfdis/CLionProjects/restAPI/server.key";
  Server server(context, cert, key);
  server.router_.add_path("GET", "/cat", [](const auto& req, auto& res) {
      res.result(http::status::ok);
      res.body() = "Hello, " + req.body() + "!";
      res.set(http::field::content_type, "text/plain");
  });
  server.run();
  context.run();
}
