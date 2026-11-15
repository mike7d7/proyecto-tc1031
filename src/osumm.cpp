#include "../headers/osumm.hpp"
#include "../external-libs/rapidcsv.h"
#include <tuple>

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
    user_ids.push_back(std::make_tuple(user_ids_temp[i], pp_values[i]));
  }
}

// Ordena usuarios en base a su habilidad (menor a mayor)
void OsuMM::sort_users_by_skill() {
  std::vector<std::tuple<int, int>> buffer(user_ids.size());

  for (int shift = 0; shift < 32; shift += 8) {
    uint32_t radix_array[256] = {};

    for (const std::tuple<int, int> &user_id : user_ids) {
      float pp = std::get<1>(user_id);

      // Cast bytes from pp_value (float) as an int
      uint32_t bits;
      bits = *(uint32_t *)&pp;

      uint32_t radix_index = (bits >> shift) & 0xFF;
      radix_array[radix_index] += 1;
    }

    // Calculate starting positions for each bucket value in the buffer
    uint32_t position_in_buffer[256];
    position_in_buffer[0] = 0;

    for (int i = 1; i < 256; i++) {
      position_in_buffer[i] = position_in_buffer[i - 1] + radix_array[i - 1];
    }

    // Move values into buffer
    for (const std::tuple<int, int> &user_id : user_ids) {
      float pp = std::get<1>(user_id);

      uint32_t bits;
      bits = *(uint32_t *)&pp;

      const uint32_t bucket = (bits >> shift) & 0xFF;

      uint32_t current_index = position_in_buffer[bucket];
      buffer[current_index] = user_id;

      position_in_buffer[bucket] = position_in_buffer[bucket] + 1;
    }
    user_ids = buffer;
  }
}

User OsuMM::search_user(uint32_t user_to_find) {
  return users.at(user_to_find);
}
std::unordered_map<int, User> &OsuMM::get_users() { return users; };
std::vector<std::tuple<int, int>> &OsuMM::get_user_ids() { return user_ids; }
