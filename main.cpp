#include "headers/user.hpp"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <ostream>
#include <rapidcsv.h>
#include <sstream>
#include <string>
#include <vector>

// Funciones de ayuda sacadas de proyecto anterior:
// https://github.com/mike7d7/proyecto-TC1030/blob/main/main.cpp
void enter_to_continue() {
  std::cout << "Presiona enter para continuar" << std::endl;
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  std::cin.get();
}

// Simula limpiar la pantalla mediante 100 newlines.
void clear() {
  for (int i = 0; i < 100; i++) {
    std::cout << std::endl;
  }
}

// Muestra el menú principal del programa
void main_menu() {
  clear();
  std::cout << "Ingresa una opción" << std::endl;
  std::cout << "1.- Mostrar Jugadores" << std::endl;
  std::cout << "2.- Mostrar información de un jugador" << std::endl;
  std::cout << "3.- Encontrar oponente" << std::endl;
  std::cout << "4.- Salir" << std::endl;
}

// Verifica que el input sea un int válido y que esté dentro del rango
// especificado. Lógica para verificar cast de string a int sacado de:
// https://cplusplus.com/forum/beginner/283248/#msg1226145
int get_int(int min_val, int max_val) {
  int number;
  while (!(std::cin >> number)) {
    std::cout << "Entrada inválida, intenta otra vez." << std::endl;
    std::cin.clear(); // clear fail flag
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                    '\n'); // discard the whole line
  }
  if (number < min_val || number > max_val) {
    std::cout << "Entrada inválida, intenta otra vez." << std::endl;
    std::cin.clear(); // clear fail flag
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                    '\n'); // discard the whole line
    number = get_int(min_val, max_val);
  }
  return number;
}

std::string print_users(const std::vector<User> &users) {
  std::stringstream output_string;
  for (int i = 0; i < 40; i++) {
    output_string << "id: " << users[i].get_user_id()
                  << "   pp: " << users[i].get_pp_value() << std::endl;
  }
  return output_string.str();
}

void sort_users_by_skill(std::vector<User> &users) {
  std::sort(users.begin(), users.end(), [](const User &a, const User &b) {
    if (a.get_pp_value() != b.get_pp_value()) {
      return a.get_pp_value() < b.get_pp_value();
    }
    return a.get_playtime() < b.get_playtime();
  });
}

int main() {
  rapidcsv::Document doc("csv/osu_user_stats_202609212000.csv");
  rapidcsv::Document sample_users("csv/sample_users_202609212000.csv",
                                  rapidcsv::LabelParams(0, 0));

  std::vector<uint32_t> user_ids = doc.GetColumn<uint32_t>("user_id");
  std::vector<double> pp_values = doc.GetColumn<double>("rank_score");
  std::vector<uint32_t> user_playtimes =
      doc.GetColumn<uint32_t>("total_seconds_played");

  std::vector<User> users(10000);
  for (int i = 0; i < user_ids.size(); i++) {
    std::string username = sample_users.GetCell<std::string>(
        "username", std::to_string(user_ids[i]));
    users[i] = User(username, user_ids[i], pp_values[i], user_playtimes[i]);
  }

  int main_menu_option = 0;
  while (main_menu_option != -1) {
    main_menu();
    main_menu_option = get_int(1, 4);

    switch (main_menu_option) {
    case 1: {
      std::cout << print_users(users) << std::endl;
      enter_to_continue();
      break;
    }
    case 2: {
      clear();
      std::cout << "Ingresa el ID del usuario: ";
      uint32_t user_to_find = get_int(0, INT32_MAX);

      // Código para encontrar User por atributo user_id
      // inspirado por https://stackoverflow.com/a/40777532
      auto found_user =
          std::find_if(users.begin(), users.end(), [&](const User &u) {
            return u.get_user_id() == user_to_find;
          });
      if (found_user != users.end()) {
        std::cout << found_user->get_user_info() << std::endl;
      } else {
        std::cout << "El usuario con ID: " << user_to_find << " no existe."
                  << std::endl;
      }
      enter_to_continue();
      break;
    }
    case 3: {
      clear();
      std::cout << "Ingresa el ID del usuario: ";
      uint32_t user_to_find = get_int(0, INT32_MAX);

      auto found_user =
          std::find_if(users.begin(), users.end(), [&](const User &u) {
            return u.get_user_id() == user_to_find;
          });
      if (found_user != users.end()) {
        sort_users_by_skill(users);
        found_user =
            std::find_if(users.begin(), users.end(), [&](const User &u) {
              return u.get_user_id() == user_to_find;
            });
        int user_index = std::distance(users.begin(), found_user);
        int rival_index;
        if (user_index > 0) {
          rival_index = user_index - 1;
        } else {
          rival_index = user_index + 1;
        }
        std::cout << "Rival seleccionado: " << std::endl
                  << users[rival_index].get_user_info() << std::endl;
      } else {
        std::cout << "El usuario con ID: " << user_to_find << " no existe."
                  << std::endl;
      }
      enter_to_continue();
      break;
    }
    default: {
      main_menu_option = -1;
    }
    }
  }
}
