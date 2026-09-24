#include "../headers/user.hpp"
#include <ostream>
#include <sstream>
#include <string>

uint32_t User::get_user_id() const { return user_id; }
double User::get_pp_value() const { return pp_value; }

std::string User::get_user_info() const {
  std::stringstream output_string;
  output_string << "Id: " << user_id << std::endl
                << "Nombre: " << username << std::endl
                << "Puntos de rendimiento: " << pp_value << std::endl
                << "Tiempo de juego (horas): " << playtime / 3600 << std::endl;
  return output_string.str();
}

uint32_t User::get_playtime() const { return playtime; };
