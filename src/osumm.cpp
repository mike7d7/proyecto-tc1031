#include "../headers/osumm.hpp"
#include <algorithm>
#include <sstream>

// Regresa string con lista de usuarios para imprimir (truncado a 40 usuarios)
std::string OsuMM::print_users() const {
  std::stringstream output_string;
  for (int i = 0; i < 40; i++) {
    output_string << "User: " << users[i].get_username()
                  << "   id: " << users[i].get_user_id() << std::endl;
  }
  return output_string.str();
}

// Ordena usuarios en base a su habilidad (menor a mayor)
void OsuMM::sort_users_by_skill() {
  std::vector<User> buffer(users.size());

  for (int shift = 0; shift < 32; shift += 8) {
    uint32_t radix_array[256] = {};

    for (const User &user : users) {
      float id = user.get_pp_value();

      // Cast bytes from pp_value (float) as an int
      uint32_t bits;
      bits = *(uint32_t *)&id;

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
    for (const User &user : users) {
      float id = user.get_pp_value();

      uint32_t bits;
      bits = *(uint32_t *)&id;

      const uint32_t bucket = (bits >> shift) & 0xFF;

      uint32_t current_index = position_in_buffer[bucket];
      buffer[current_index] = user;

      position_in_buffer[bucket] = position_in_buffer[bucket] + 1;
    }
    users = buffer;
  }
}

std::vector<User>::iterator OsuMM::search_user(uint32_t user_to_find) {
  // Código para encontrar User por atributo user_id
  // inspirado por https://stackoverflow.com/a/40777532
  auto found_user =
      std::find_if(users.begin(), users.end(), [&](const User &u) {
        return u.get_user_id() == user_to_find;
      });
  return found_user;
}

std::vector<User> &OsuMM::get_users() { return users; }
