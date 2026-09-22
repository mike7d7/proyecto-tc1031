#include "headers/user.hpp"
#include <cstdint>
#include <iostream>
#include <rapidcsv.h>

int main() {
  rapidcsv::Document doc("csv/osu_user_stats_202609212000.csv");

  std::vector<uint32_t> user_ids = doc.GetColumn<uint32_t>("user_id");
  std::vector<double> pp_values = doc.GetColumn<double>("rank_score");

  std::vector<User> users(10000);

  for (int i = 0; i < user_ids.size(); i++) {
    users[i] = User(user_ids[i], pp_values[i]);
  }

  for (int i = 0; i < 100; i++) {
    std::cout << "id: " << users[i].get_user_id()
              << "   pp: " << users[i].get_pp_value() << std::endl;
  }
}
