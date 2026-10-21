#ifndef OSUMM_H
#define OSUMM_H

#include "user.hpp"
#include <cstdint>
#include <tuple>
#include <unordered_map>
#include <vector>
class OsuMM {
private:
  std::unordered_map<int, User> users;
  std::vector<std::tuple<int, int>> user_ids; // <id, pp_value>

public:
  std::string print_users() const;
  void sort_users_by_skill();
  OsuMM() : users() {};
  OsuMM(int users_vector_size) : users(users_vector_size) {};
  User search_user(uint32_t);

  std::unordered_map<int, User> &get_users();
  std::vector<std::tuple<int, int>> &get_user_ids();
};
#endif
