#ifndef USER_H
#define USER_H

#include <cstdint>
class User {
private:
  uint32_t user_id;
  double pp_value; // rank_score in db

public:
  User() = default;
  User(uint32_t user_id, double pp_value)
      : user_id(user_id), pp_value(pp_value) {};

  uint32_t get_user_id() const;
  double get_pp_value() const;
};

#endif
