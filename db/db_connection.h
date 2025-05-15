//
// Created by shfdis on 5/14/25.
//

#ifndef DB_CONNECTION_H
#define DB_CONNECTION_H

#include <boost/shared_ptr.hpp>
#include <sqlite3.h>
#include "request.h"

class DatabaseConnection {
public:
    enum Error {
        error,
        ok
    };

private:
    sqlite3 *db = nullptr;
    std::function<void(Error, std::vector<std::string>, std::vector<std::string>)> callback_;

public:
    explicit DatabaseConnection(const std::string &db_path);

    DatabaseConnection(const DatabaseConnection &) = delete;

    DatabaseConnection &operator=(const DatabaseConnection &) = delete;

    DatabaseConnection(DatabaseConnection &&) = delete;

    DatabaseConnection &operator=(DatabaseConnection &&) = delete;

    ~DatabaseConnection();

    std::vector<std::vector<std::string> > Exec(const RequestValue &req, size_t columns_) const;
};


#endif //DB_CONNECTION_H
