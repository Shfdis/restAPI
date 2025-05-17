//
// Created by shfdis on 5/14/25.
//

#include "db_connection.h"

#include <cstring>
#include <iostream>

DatabaseConnection::DatabaseConnection(const std::string &db_path) {
  int result = sqlite3_open(db_path.c_str(), &db);
  if (result != SQLITE_OK) {
    std::cerr << sqlite3_errmsg(db) << '\n';
    throw new std::runtime_error("Failed to open database connection");
  }
}

DatabaseConnection::~DatabaseConnection() { sqlite3_close(db); }

std::vector<std::vector<std::string> > DatabaseConnection::Exec(
    const RequestValue &req, size_t columns_) const {
  if (!req.is_ready()) {
    throw new std::runtime_error("Request is not full");
  }
  sqlite3_stmt *stmt = NULL;
  int value = sqlite3_prepare_v2(db, req.ToString().c_str(), -1, &stmt, NULL);
  std::vector<std::vector<std::string> > result;
  if (value == SQLITE_OK) {
    while (sqlite3_step(stmt) == SQLITE_ROW) {
      result.push_back(std::vector<std::string>());
      for (size_t i = 0; i < columns_; i++) {
        const char *val =
            reinterpret_cast<const char *>(sqlite3_column_text(stmt, i));
        result.back().push_back(std::string(val, strlen(val)));
      }
    }
    return result;
  } else {
    throw std::runtime_error("Failed to prepare statement");
  }
}
