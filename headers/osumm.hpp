#ifndef OSUMM_H
#define OSUMM_H

#include "user.hpp"
#include <cstdint>
#include <unordered_map>
#include <vector>
class OsuMM {
private:
  std::unordered_map<int, User> users;
  std::vector<int> user_ids; // id

public:
  void sort_users_by_skill(bool);
  OsuMM();

  bool save_data();

  User search_user(uint32_t);
  void delete_user(uint32_t);

  std::unordered_map<int, User> &get_users();
  std::vector<int> &get_user_ids();
};
#endif
