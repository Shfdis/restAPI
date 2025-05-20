//
// Created by shfdis on 5/19/25.
//

#include "messages.h"

#include "../db/db_connection.h"
#include "../db/request.h"
#include "settings.h"
bool GroupMutexes::is_created = false;
void create_group(const std::vector<std::string>& members,
                  const std::string& name,
                  std::shared_ptr<GroupMutexes> mutexes) {
  std::string members_string;
  for (auto i : members) {
    members_string += i + ",";
  }
  if (!members_string.empty()) {
    members_string.pop_back();
  }
  DatabaseConnection db(Settings::DB_PATH);
  for (auto i : members) {
    RequestValue rq = SelectRequest::Select("*")
                          .From("passwords")
                          .Where(std::string("id=") + "\"" + i + "\"")
                          .Finalize();
    if (db.Exec(rq, 2).size() == 0) {
      throw std::logic_error("No such user");
    }
  }
  std::lock_guard lock(mutexes->create_group_mutex);
  RequestValue rq =
      InsertRequest::Insert("groups")
          .Columns(std::vector<std::string>{"members", "name"})
          .Values(std::vector{"\"" + members_string + "\"", "\"" + name + "\""})
          .Finalize();
  db.Exec(rq, 3);
  uint64_t id = std::stoull(db.Exec(SelectRequest::Select("id")
                                        .From("groups")
                                        .Where("name=\"" + name + "\"")
                                        .Finalize(),
                                    1)
                                .back()[0]);
  for (auto i : members) {
    RequestValue rq =
        InsertRequest::Insert("users_groups")
            .Columns(std::vector<std::string>{"id", "group_id"})
            .Values(std::vector{"\"" + i + "\"", std::to_string(id)})
            .Finalize();
    db.Exec(rq, 2);
  }
}
