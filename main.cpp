#include "external-libs/rapidcsv.h"
#include "headers/osumm.hpp"
#include "headers/user.hpp"
#include <cstdint>
#include <iostream>
#include <ostream>
#include <string>
#include <tuple>
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

int main() {
  // Cargar archivos csv
  rapidcsv::Document doc("csv/osu_user_stats_202609212000.csv");
  rapidcsv::Document sample_users("csv/sample_users_202609212000.csv",
                                  rapidcsv::LabelParams(0, 0));

  // Arreglos temporales con atributos
  std::vector<uint32_t> user_ids_temp = doc.GetColumn<uint32_t>("user_id");
  std::vector<float> pp_values = doc.GetColumn<float>("rank_score");
  std::vector<uint32_t> user_playtimes =
      doc.GetColumn<uint32_t>("total_seconds_played");

  // Crear hashmap de users con los arreglos temporales
  OsuMM osumm = OsuMM(user_ids_temp.size());
  std::unordered_map<int, User> &users = osumm.get_users();
  for (int i = 0; i < user_ids_temp.size(); i++) {
    std::string username = sample_users.GetCell<std::string>(
        "username", std::to_string(user_ids_temp[i]));
    users.insert({user_ids_temp[i], User(username, user_ids_temp[i],
                                         pp_values[i], user_playtimes[i])});
  }

  // Crear vector con id y pp_value de Users para poder ordenar y encontrar
  // oponente
  std::vector<std::tuple<int, int>> &user_ids = osumm.get_user_ids();
  for (int i = 0; i < user_ids_temp.size(); i++) {
    user_ids.push_back(std::make_tuple(user_ids_temp[i], pp_values[i]));
  }

  int main_menu_option = 0;
  while (main_menu_option != -1) {
    main_menu();
    main_menu_option = get_int(1, 4);

    switch (main_menu_option) {
      // Opción de imprimir lista de usuarios.
    case 1: {
      std::cout << osumm.print_users() << std::endl;
      enter_to_continue();
      break;
    }
      // Opción para mostrar información de un usuario
    case 2: {
      clear();
      std::cout << "Ingresa el ID del usuario: ";
      uint32_t user_to_find = get_int(0, INT32_MAX);
      try {
        auto found_user = osumm.search_user(user_to_find);
        std::cout << found_user.get_user_info() << std::endl;
      } catch (const std::out_of_range &ex) {
        std::cout << "El usuario con ID: " << user_to_find << " no existe."
                  << std::endl;
      }

      enter_to_continue();
      break;
    }
      // Opción para encontrar oponente
    case 3: {
      clear();
      std::cout << "Ingresa el ID del usuario: ";
      uint32_t user_to_find = get_int(0, INT32_MAX);

      try {
        auto found_user = osumm.search_user(user_to_find);

        osumm.sort_users_by_skill();
        int user_index = 0;
        bool found = false;
        while (!found) {
          if (std::get<0>(user_ids[user_index]) != user_to_find) {
            user_index++;
          } else {
            found = true;
          }
        }
        int rival_index;
        if (user_index > 0) {
          rival_index = user_index - 1;
        } else {
          rival_index = user_index + 1;
        }
        try {
          auto rival_user = std::get<0>(user_ids[rival_index]);
          std::cout << "Jugador buscado: " << found_user.get_user_info()
                    << std::endl;

          std::cout << "Rival seleccionado: " << std::endl
                    << osumm.search_user(rival_user).get_user_info()
                    << std::endl;
        } catch (const std::out_of_range &ex) {
          std::cout << "No deberia existir este error" << std::endl;
        }
      } catch (const std::out_of_range &ex) {
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
