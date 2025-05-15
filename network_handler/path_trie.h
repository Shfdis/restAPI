//
// Created by shfdis on 5/14/25.
//

#ifndef PATH_TRIE_H
#define PATH_TRIE_H
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

template <typename request>
class PathTrie {
 private:
  struct Node {
    std::unordered_map<std::string, std::shared_ptr<Node>> children;
    request *req = nullptr;
    Node() { req = nullptr; }
    ~Node() { delete req; };
  };

  std::shared_ptr<Node> root = std::make_shared<Node>();

  static std::vector<std::string> path(const std::string &pth) {
    std::vector<std::string> paths = {""};
    for (auto p : pth) {
      if (p == '/') {
        while (!paths.empty() && paths.back().empty()) {
          paths.pop_back();
        }
        paths.emplace_back("");
      } else {
        paths.back().push_back(p);
      }
    }
    while (!paths.empty() && paths.back().empty()) {
      paths.pop_back();
    }
    return paths;
  }

 public:
  PathTrie() = default;

  ~PathTrie() = default;

  PathTrie(const PathTrie &) = delete;

  PathTrie(PathTrie &&) = delete;

  void Add(const std::string &path, request req) {
    std::vector<std::string> paths = PathTrie::path(path);
    std::weak_ptr<Node> node = std::weak_ptr<Node>(root);
    for (auto p : paths) {
      if (node.lock()->children.find(p) == node.lock()->children.end()) {
        node.lock()->children[p] = std::make_shared<Node>();
      }
      node = node.lock()->children[p];
    }
    if (node.lock()->req != nullptr) {
      throw std::logic_error("Path already exists");
    }
    node.lock()->req = new request(req);
  }

  request *operator[](const std::string &path) {
    std::vector<std::string> paths = PathTrie::path(path);
    std::weak_ptr<Node> node = std::weak_ptr<Node>(root);
    for (auto p : paths) {
      if (node.lock()->children.find(p) == node.lock()->children.end()) {
        return nullptr;
      }
      node = node.lock()->children[p];
    }
    return node.lock()->req;
  }
};

#endif  // PATH_TRIE_H
