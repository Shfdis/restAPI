//
// Created by shfdis on 5/19/25.
//

#ifndef MESSAGES_H
#define MESSAGES_H
#include <cstdint>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <vector>

struct Group {
  uint64_t id;
  std::vector<std::string> members;
};

struct Message {
  uint64_t group_id;
  uint64_t id;
  std::string user;
  std::string content;
};

struct GroupMutexes {
  static bool is_created;
  std::mutex create_group_mutex;
  GroupMutexes() {
    if (is_created) {
      throw std::runtime_error("is already created");
    }
    is_created = true;
  }
};
void create_group(const std::vector<std::string>& members, const std::string& name, std::shared_ptr<GroupMutexes> mutexes);
#endif  // MESSAGES_H
