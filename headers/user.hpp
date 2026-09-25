#ifndef USER_H
#define USER_H

#include <cstdint>
#include <string>
class User {
private:
  uint32_t user_id;
  std::string username;
  float pp_value;    // rank_score in db
  uint32_t playtime; // in seconds

public:
  User() = default;
  User(std::string username, uint32_t user_id, float pp_value,
       uint32_t playtime)
      : username(username), user_id(user_id), pp_value(pp_value),
        playtime(playtime) {};

  uint32_t get_user_id() const;
  float get_pp_value() const;
  std::string get_user_info() const;
  uint32_t get_playtime() const;
};

#endif
