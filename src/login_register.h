//
// Created by shfdis on 5/16/25.
//

#ifndef LOGIN_REGISTER_H
#define LOGIN_REGISTER_H
#include <exception>
#include <string>

#include "../crypto/crypto.h"
#include "../db/db_connection.h"
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
  UsernameTakenError() : std::logic_error("Username taken") {}
};
class WrongPasswordError : public std::logic_error {
 public:
  WrongPasswordError() : std::logic_error("Wrong password") {}
};
void sign_up(const std::string& username, const std::string& password,
             std::shared_ptr<LoginMutexes> login_mutexes);
// returns guid
std::string sign_in(const std::string& username, const std::string& password, std::shared_ptr<LoginMutexes> login_mutexes);

#endif  // LOGIN_REGISTER_H
