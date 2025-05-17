//
// Created by shfdis on 5/16/25.
//

#ifndef LOGIN_REGISTER_H
#define LOGIN_REGISTER_H
#include <string>
#include <exception>
#include "../db/db_connection.h"
#include "../crypto/crypto.h"
#include "settings.h"
struct LoginMutexes {
  std::mutex signin_mutex;
  static bool is_created;
  LoginMutexes() {
    if (is_created) {
      throw std::logic_error("LoginMutexes is already created");
    }
    is_created = true;
  }
};

class UsernameTakenError : public std::logic_error {
  public:
  UsernameTakenError() : std::logic_error("Username taken") {

  }
};
void sign_up(const std::string& username, const std::string& password, std::shared_ptr<LoginMutexes> login_mutexes);
// returns guid
std::string sign_in(const std::string& username, const std::string& password);

bool logout(const std::string& guid);

bool is_logged_in(const std::string& guid, const std::string& username);

#endif  // LOGIN_REGISTER_H
