#include "../headers/user.hpp"
#include <string>

uint32_t User::get_user_id() const { return user_id; }
float User::get_pp_value() const { return pp_value; }

uint32_t User::get_playtime() const { return playtime; }

std::string User::get_username() const { return username; }
