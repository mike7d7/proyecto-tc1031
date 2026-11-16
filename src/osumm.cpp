#include "../headers/osumm.hpp"
#include "../external-libs/rapidcsv.h"
#include <cstdint>
#include <string>

OsuMM::OsuMM() {
  // Cargar archivos csv
  rapidcsv::Document doc("csv/osu_user_stats_202609212000.csv");
  rapidcsv::Document sample_users("csv/sample_users_202609212000.csv",
                                  rapidcsv::LabelParams(0, 0));

  // Arreglos temporales con atributos
  std::vector<uint32_t> user_ids_temp = doc.GetColumn<uint32_t>("user_id");
  std::vector<float> pp_values = doc.GetColumn<float>("rank_score");
  std::vector<uint32_t> user_playtimes =
      doc.GetColumn<uint32_t>("total_seconds_played");

  // Preasignar memoria
  users.reserve(user_ids_temp.size());
  user_ids.reserve(user_ids_temp.size());

  // Crear hashmap de users con los arreglos temporales
  for (int i = 0; i < user_ids_temp.size(); i++) {
    std::string username = sample_users.GetCell<std::string>(
        "username", std::to_string(user_ids_temp[i]));
    users.insert({user_ids_temp[i], User(username, user_ids_temp[i],
                                         pp_values[i], user_playtimes[i])});
  }

  // Crear vector con id y pp_value de Users para poder ordenar y encontrar
  // oponente
  for (int i = 0; i < user_ids_temp.size(); i++) {
    user_ids.push_back(user_ids_temp[i]);
  }
}

// Ordena usuarios en base a un atributo seleccionado
void OsuMM::sort_users(bool desc, int attribute) {
  std::vector<int> buffer(user_ids.size());

  auto get_value_to_compare = [&](uint32_t id) {
    uint32_t result = 0;
    switch (attribute) {
    case 0: {
      result = users[id].get_user_id();
      break;
    }
    case 1: {
      break;
    }
    case 2: {
      float pp = users[id].get_pp_value();
      result = *(uint32_t *)&pp;
      break;
    }
    case 3: {
      result = users[id].get_playtime();
      break;
    }
    }
    return result;
  };

  for (int shift = 0; shift < 32; shift += 8) {
    uint32_t radix_array[256] = {};

    for (const int &user_id : user_ids) {
      uint32_t bits = get_value_to_compare(user_id);
      uint32_t radix_index = (bits >> shift) & 0xFF;
      if (desc) {
        radix_index = 255 - radix_index;
      }
      radix_array[radix_index] += 1;
    }

    // Calculate starting positions for each bucket value in the buffer
    uint32_t position_in_buffer[256];
    position_in_buffer[0] = 0;

    for (int i = 1; i < 256; i++) {
      position_in_buffer[i] = position_in_buffer[i - 1] + radix_array[i - 1];
    }

    // Move values into buffer
    for (const int &user_id : user_ids) {
      uint32_t bits = get_value_to_compare(user_id);
      uint32_t bucket = (bits >> shift) & 0xFF;
      if (desc) {
        bucket = 255 - bucket;
      }
      uint32_t current_index = position_in_buffer[bucket];
      buffer[current_index] = user_id;

      position_in_buffer[bucket] = position_in_buffer[bucket] + 1;
    }
    user_ids = buffer;
  }
}

bool OsuMM::save_data() {
  // sample_users file
  rapidcsv::Document sample_users;
  sample_users.SetColumnName(0, "user_id");
  sample_users.SetColumnName(1, "username");
  sample_users.SetColumnName(2, "user_warnings");
  sample_users.SetColumnName(3, "user_type");

  for (int i = 0; i < user_ids.size(); i++) {
    uint32_t user_index = user_ids[i];
    sample_users.SetCell<uint32_t>(0, i, user_index);
    sample_users.SetCell<std::string>(1, i,
                                      search_user(user_index).get_username());
  }

  // osu_user_stats file
  rapidcsv::Document osu_user_stats;
  osu_user_stats.SetColumnName(0, "user_id");
  osu_user_stats.SetColumnName(1, "rank_score");
  osu_user_stats.SetColumnName(2, "total_seconds_played");

  for (int i = 0; i < user_ids.size(); i++) {
    uint32_t user_index = user_ids[i];
    osu_user_stats.SetCell<uint32_t>(0, i, user_index);
    osu_user_stats.SetCell<float>(1, i, search_user(user_index).get_pp_value());
    osu_user_stats.SetCell<uint32_t>(2, i,
                                     search_user(user_index).get_playtime());
  }

  try {
    sample_users.Save("csv/sample_users.csv");
    osu_user_stats.Save("csv/osu_user_stats.csv");
  } catch (const std::exception &e) {
    return false;
  }
  return true;
}

User OsuMM::search_user(uint32_t user_to_find) {
  return users.at(user_to_find);
}

void OsuMM::delete_user(uint32_t target_user) {
  users.erase(target_user);

  for (int i = 0; i < user_ids.size(); i++) {
    if (target_user == user_ids[i]) {
      user_ids.erase(user_ids.begin() + i);
      return;
    }
  }
}

std::unordered_map<int, User> &OsuMM::get_users() { return users; };
std::vector<int> &OsuMM::get_user_ids() { return user_ids; }
