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
std::vector<std::string> split(std::string str, char sep = ',') {
  std::vector<std::string> answer = {""};
  for (auto i : str) {
    if (i == sep) {
      answer.emplace_back();
    } else {
      answer.back().push_back(i);
    }
  }
  if (answer.size() == 1 && answer.back().empty()) {
    return {};
  }
  return answer;
}
std::vector<std::string> get_members_from_group(uint64_t group_id) {
  RequestValue rq = SelectRequest::Select("members")
                        .From("groups")
                        .Where("id = " + std::to_string(group_id))
                        .Finalize();
  DatabaseConnection connection(Settings::DB_PATH);
  auto answer = connection.Exec(rq, 1);
  if (answer.empty()) {
    throw std::logic_error("No such group");
  }
  return split(answer[0][0]);
}
void send_message(const Message& message,
                  std::shared_ptr<GroupMutexes> mutexes) {
  auto members = get_members_from_group(message.group_id);
  if (std::find(members.begin(), members.end(), message.user) ==
      members.end()) {
    throw std::logic_error("No such user in this group");
  }
  {
    DatabaseConnection connection(Settings::DB_PATH);
    std::lock_guard(mutexes->group_mutexes[message.group_id]);
    RequestValue rq =
        SelectRequest::Select("id")
            .From("messages")
            .Where("group_id = " + std::to_string(message.group_id))
            .OrderByDesc("id")
            .Limit(1)
            .Finalize();
    auto ans = connection.Exec(rq, 1);
    int id = 0;
    if (!ans.empty()) {
      id = std::stoull(ans[0][0]) + 1;
    }
    rq = InsertRequest::Insert("messages")
             .Columns(
                 std::vector<std::string>{"id", "group_id", "user", "content"})
             .Values(std::vector<std::string>{
                 std::to_string(id), std::to_string(message.group_id),
                 "\"" + message.user + "\"", "\"" + message.content + "\""})
             .Finalize();
    connection.Exec(rq, 2);
  }
}

std::string get_all_groups(std::string id) {
  std::string answer;
  RequestValue rq = SelectRequest::Select("group_id")
                        .From("users_groups")
                        .Where(std::string("id = ") + "\"" + id + "\"")
                        .Finalize();
  DatabaseConnection connection(Settings::DB_PATH);
  auto values = connection.Exec(rq, 1);
  for (auto i : values) {
    answer += i[0] + ",";
  }
  if (!answer.empty()) {
    answer.pop_back();
  }
  return answer;
}

std::string get_group_info(uint64_t id) {
  std::string id_str = std::to_string(id);
  std::string answer = "\"group_id\": " + id_str + ',';
  DatabaseConnection connection(Settings::DB_PATH);
  RequestValue rq = SelectRequest::Select("*")
  .From("groups")
  .Where("id = " + id_str)
  .Finalize();
  auto values = connection.Exec(rq, 3);
  if (values.empty()) {
    throw std::logic_error("No such group");
  }
  std::vector<std::string> users = split(values[0][1]);
  answer = "\"members\": [";
  for (auto i : users) {
    answer += "\"" + i + "\",";
  }
  if (!users.empty()) {
    answer.pop_back();
  }
  answer.push_back(']');
  answer += ", \"name\": \"" + values[0][2] + "\"";
  return answer;
}
