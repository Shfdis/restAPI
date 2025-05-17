#include "network_handler/server.h"
#include "src/add_requests.h"
int main() {
  asio::io_context context;
  std::string cert = "/home/shfdis/CLionProjects/restAPI/server.crt";
  std::string key = "/home/shfdis/CLionProjects/restAPI/server.key";
  Server server(context, cert, key);
  add_requests(server.router_);
  server.run();
  context.run();
}
