#ifndef OSUMM_H
#define OSUMM_H

#include "user.hpp"
#include <cstdint>
#include <vector>
class OsuMM {
private:
  std::vector<User> users;

public:
  std::string print_users() const;
  void sort_users_by_skill();
  OsuMM() : users() {};
  OsuMM(int users_vector_size) : users(users_vector_size) {};
  std::vector<User>::iterator search_user(uint32_t);

  std::vector<User> &get_users();
};
#endif
