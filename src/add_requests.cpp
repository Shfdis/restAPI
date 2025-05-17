//
// Created by shfdis on 5/16/25.
//
#include "add_requests.h"

#include "login_register.h"

void add_requests(Router& router) {
  std::shared_ptr<LoginMutexes> login_mutexes =
      std::make_shared<LoginMutexes>();
  router.add_path("GET", "/", [](const auto& req, auto& res) {
    res.result(http::status::ok);
    res.body() = "{\"status\": \"up\"}";
    res.set("content-type", "application/json");
  });
  router.add_path("POST", "/signup", [login_mutexes](const auto& req, auto& res) {
    res.set("content-type", "application/json");
    try {
      std::string body = req.body();
      std::string id;
      std::string password;
      try {
        boost::json::value json = boost::json::parse(body);
        id = json.as_object().at("username").as_string();
        password = json.as_object().at("password").as_string();
      } catch (...) {
        throw std::logic_error("Failed to parse body");
      }
      sign_up(id, password, login_mutexes);
      res.result(http::status::ok);
      res.body() = "{\"status\": \"OK\"}";
    } catch (UsernameTakenError& e) {
      res.result(http::status::ok);
      res.body() = std::string("{\"status\":\"") + e.what() + "\"}";
    } catch (const std::logic_error& e) {
      res.result(http::status::bad_request);
      res.body() = std::string("{\"status\":\"") + e.what() + "\"}";
    } catch (const std::runtime_error& e) {
      res.result(http::status::internal_server_error);
      res.body() = std::string("{\"status\":\"internal server error\"}");
    }
  });
}
