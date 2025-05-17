//
// Created by shfdis on 5/16/25.
//

#include "login_register.h"
bool LoginMutexes::is_created = false;
void sign_up(const std::string& username, const std::string& password,
             std::shared_ptr<LoginMutexes> login_mutexes) {
  DatabaseConnection connection(Settings::DB_PATH);
  RequestValue in_db = SelectRequest::Select("*")
                           .From("passwords")
                           .Where("id==\"" + username + "\"")
                           .Finalize();
  std::lock_guard lock(login_mutexes->signin_mutex);
  auto users = connection.Exec(in_db, 2);
  if (users.empty()) {
    RequestValue add_user =
        InsertRequest::Insert("passwords")
            .Columns(std::vector<std::string>{"id", "hash"})
            .Values(std::vector{"\"" + username + "\"",
                                "\"" + GenerateHash(password) + "\""})
            .Finalize();
    connection.Exec(add_user, 2);
  } else {
    throw std::logic_error("Username already taken");
  }
}
