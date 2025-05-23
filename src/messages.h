//
// Created by shfdis on 5/19/25.
//

#ifndef MESSAGES_H
#define MESSAGES_H
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <vector>


struct Message {
  uint64_t group_id;
  std::string user;
  std::string content;
};

struct GroupMutexes {
  static bool is_created;
  std::mutex create_group_mutex;
  std::map<uint64_t, std::mutex> group_mutexes;
  GroupMutexes() {
    if (is_created) {
      throw std::runtime_error("is already created");
    }
    is_created = true;
  }
};
void create_group(const std::vector<std::string>& members, const std::string& name, std::shared_ptr<GroupMutexes> mutexes);

void send_message(const Message& message, std::shared_ptr<GroupMutexes> mutexes);

std::string get_all_groups(std::string id);

std::string get_group_info(uint64_t group_id);
#endif  // MESSAGES_H
