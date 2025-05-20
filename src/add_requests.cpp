//
// Created by shfdis on 5/16/25.
//
#include "add_requests.h"

#include "login_register.h"
#include "messages.h"
void add_requests(Router& router) {
  std::shared_ptr<LoginMutexes> login_mutexes =
      std::make_shared<LoginMutexes>();
  router.add_path("GET", "/", [](const auto& req, auto& res) {
    res.result(http::status::ok);
    res.body() = "{\"status\": \"up\"}";
    res.set("content-type", "application/json");
  });
  router.add_path(
      "POST", "/signup", [login_mutexes](const auto& req, auto& res) {
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
  router.add_path("GET", "/token", [login_mutexes](const auto& req, auto& res) {
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
      std::string guid = "\"" + sign_in(id, password, login_mutexes) + "\"";
      res.result(http::status::ok);
      res.body() = "{\"status\":\"OK\", \"guid\":" + guid + "}";
    } catch (const std::logic_error& e) {
      res.result(http::status::bad_request);
      res.body() = std::string("{\"status\":\"") + e.what() + "\"}";
    } catch (const std::runtime_error& e) {
      res.result(http::status::internal_server_error);
      res.body() = std::string("{\"status\":\"internal server error\"}");
    }
  });
  std::shared_ptr<GroupMutexes> group_mutexes =
      std::make_shared<GroupMutexes>();
  router.add_path(
      "POST", "/group", [group_mutexes](const auto& req, auto& res) {
        res.set("content-type", "application/json");
        try {
          std::string body = req.body();
          std::string id;
          std::string guid;
          std::string name;
          std::vector<std::string> members;
          try {
            boost::json::value json = boost::json::parse(body);
            id = json.as_object().at("username").as_string();
            guid = json.as_object().at("guid").as_string();
            name = json.as_object().at("name").as_string();
            for (auto i : json.as_object().at("members").as_array()) {
              members.push_back(static_cast<std::string>(i.as_string()));
            }
          } catch (...) {
            throw std::logic_error("Failed to parse body");
          }
          if (!is_logged_in(id, guid)) {
            throw std::logic_error("Failed to login");
          }
          if (std::find(members.begin(), members.end(), id) == members.end()) {
            throw std::logic_error("Group founder is not a member");
          }
          create_group(members, name, group_mutexes);
          res.result(http::status::ok);
          res.body() = "{\"status\":\"OK\"}";
        } catch (const std::logic_error& e) {
          res.result(http::status::bad_request);
          res.body() = std::string("{\"status\":\"") + e.what() + "\"}";
        } catch (const std::runtime_error& e) {
          res.result(http::status::internal_server_error);
          res.body() = std::string("{\"status\":\"internal server error\"}");
        }
      });
}
