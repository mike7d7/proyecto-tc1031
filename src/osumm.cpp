#include "../headers/osumm.hpp"
#include <sstream>
#include <tuple>

// Regresa string con lista de usuarios para imprimir (truncado a 40 usuarios)
std::string OsuMM::print_users() const {
  std::stringstream output_string;
  int count = 0;
  for (auto it = users.begin(); it != users.end() && count < 40;
       ++it, ++count) {
    output_string << "Key: " << it->first
                  << ", Value: " << it->second.get_username() << std::endl;
  }
  return output_string.str();
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
