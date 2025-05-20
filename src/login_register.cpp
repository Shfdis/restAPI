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
                           .Where("id=\"" + username + "\"")
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
std::string sign_in(const std::string& username, const std::string& password,
                    std::shared_ptr<LoginMutexes> login_mutexes) {
  DatabaseConnection connection(Settings::DB_PATH);
  RequestValue hash = SelectRequest::Select("hash")
                          .From("passwords")
                          .Where("id=\"" + username + "\"")
                          .Finalize();
  auto users = connection.Exec(hash, 1);
  if (users.empty()) {
    throw std::logic_error("No such user");
  }
  std::string hsh = users[0][0];
  if (!VerifyHash(password, hsh)) {
    throw WrongPasswordError();
  }
  std::string guid = GenerateUuid();
  RequestValue insert_guid =
      InsertRequest::Insert("guids")
          .Columns(std::vector<std::string>{"id", "guid"})
          .Values(std::vector{"\"" + username + "\"", "\"" + guid + "\""})
          .Finalize();
  connection.Exec(insert_guid, 2);
  return guid;
}

bool is_logged_in(const std::string& username, const std::string& guid) {
  DatabaseConnection connection(Settings::DB_PATH);
  RequestValue in_db =
      SelectRequest::Select("*")
          .From("guids")
          .Where("id=\"" + username + "\"" + "and guid=\"" + guid + "\"")
          .Finalize();
  auto user = connection.Exec(in_db, 2);
  return !user.empty();
}
