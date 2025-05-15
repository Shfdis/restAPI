add_library(db
        db_connection.cpp
        db_connection.h
        request.cpp
        request.h)
find_package(SQLite3 REQUIRED)
target_link_libraries(db SQLite::SQLite3)