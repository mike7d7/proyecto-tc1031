#include "external-libs/imgui/backends/imgui_impl_sdl3.h"
#include "external-libs/imgui/backends/imgui_impl_sdlgpu3.h"
#include "external-libs/rapidcsv.h"
#include "headers/osumm.hpp"
#include "headers/user.hpp"
#include "imgui.h"
#include <SDL3/SDL.h>
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <sstream>
#include <stdio.h>  // printf, fprintf
#include <stdlib.h> // abort
#include <string>
#include <tuple>
#include <vector>

// This example doesn't compile with Emscripten yet! Awaiting SDL3 support.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

void show_user_info(const User &found_user, const char *id_name) {
  uint32_t total_minutes = found_user.get_playtime() / 60;
  uint32_t days = total_minutes / (24 * 60);
  uint32_t hours = (total_minutes / 60) % 24;
  uint32_t minutes = total_minutes % 60;

  ImGui::BeginChild(id_name, ImVec2(0, 160.0f), ImGuiChildFlags_Borders);

  ImGui::TextUnformatted(found_user.get_username().c_str());
  ImGui::TextDisabled("User ID: %u", found_user.get_user_id());
  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  if (ImGui::BeginTable("PlayerStats", 2, ImGuiTableFlags_SizingStretchSame)) {
    ImGui::TableNextColumn();
    ImGui::BeginChild("PPCard", ImVec2(0, 90), ImGuiChildFlags_Borders);
    ImGui::TextDisabled("Puntos de rendimiento (PP)");
    ImGui::SetWindowFontScale(1.5f);
    ImGui::Text("%.2f", found_user.get_pp_value());
    ImGui::SetWindowFontScale(1.0f);
    ImGui::EndChild();

    ImGui::TableNextColumn();
    ImGui::BeginChild("PlaytimeCard", ImVec2(0, 90), ImGuiChildFlags_Borders);
    ImGui::TextDisabled("Tiempo de juego");
    ImGui::Text("%u días, %u horas, %u minutos", days, hours, minutes);
    ImGui::EndChild();

    ImGui::EndTable();
  }

  ImGui::EndChild();
}

// Main code
int main(int, char **) {
  // Setup SDL
  // Use wayland on Linux, on other platforms comment this line
  SDL_SetHintWithPriority(SDL_HINT_VIDEO_DRIVER, "wayland", SDL_HINT_OVERRIDE);

  // [If using SDL_MAIN_USE_CALLBACKS: all code below until the main loop starts
  // would likely be your SDL_AppInit() function]
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
    printf("Error: SDL_Init(): %s\n", SDL_GetError());
    return 1;
  }

  // Create SDL window graphics context
  float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
  SDL_WindowFlags window_flags =
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
  SDL_Window *window = SDL_CreateWindow("Dear ImGui SDL3+SDL_GPU example",
                                        (int)(1280 * main_scale),
                                        (int)(800 * main_scale), window_flags);
  if (window == nullptr) {
    printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
    return 1;
  }
  SDL_SetWindowPosition(window, 0, 0);
  SDL_ShowWindow(window);

  // Create GPU Device
  SDL_GPUDevice *gpu_device = SDL_CreateGPUDevice(
      SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL |
          SDL_GPU_SHADERFORMAT_MSL | SDL_GPU_SHADERFORMAT_METALLIB,
      true, nullptr);
  if (gpu_device == nullptr) {
    printf("Error: SDL_CreateGPUDevice(): %s\n", SDL_GetError());
    return 1;
  }

  // Claim window for GPU Device
  if (!SDL_ClaimWindowForGPUDevice(gpu_device, window)) {
    printf("Error: SDL_ClaimWindowForGPUDevice(): %s\n", SDL_GetError());
    return 1;
  }
  SDL_SetGPUSwapchainParameters(gpu_device, window,
                                SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
                                SDL_GPU_PRESENTMODE_VSYNC);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGuiStyle &style = ImGui::GetStyle();
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();
  // style.FontSizeBase = 16.0f;           // Default to 13.0f when unspecified,
  // which allows crisp legacy font for non-4K screens.
  style.FrameRounding = 3.0f;
  style.GrabRounding = 3.0f;

  // Setup scaling
  style.ScaleAllSizes(
      main_scale); // Bake a fixed style scale. (until we have a solution for
                   // dynamic style scaling, changing this requires resetting
                   // Style + calling this again)
  style.FontScaleDpi =
      main_scale; // Set initial font scale. (in docking branch: using
                  // io.ConfigDpiScaleFonts=true automatically overrides this
                  // for every window depending on the current monitor)

  // Setup Platform/Renderer backends
  ImGui_ImplSDL3_InitForSDLGPU(window);
  ImGui_ImplSDLGPU3_InitInfo init_info = {};
  init_info.Device = gpu_device;
  init_info.ColorTargetFormat =
      SDL_GetGPUSwapchainTextureFormat(gpu_device, window);
  init_info.MSAASamples =
      SDL_GPU_SAMPLECOUNT_1; // Only used in multi-viewports mode.
  init_info.SwapchainComposition =
      SDL_GPU_SWAPCHAINCOMPOSITION_SDR; // Only used in multi-viewports mode.
  init_info.PresentMode = SDL_GPU_PRESENTMODE_VSYNC;
  ImGui_ImplSDLGPU3_Init(&init_info);

  // Load Fonts
  // - If fonts are not explicitly loaded, Dear ImGui will select an embedded
  // font: either AddFontDefaultVector() or AddFontDefaultBitmap().
  //   This selection is based on (style.FontSizeBase * style.FontScaleMain *
  //   style.FontScaleDpi) reaching a threshold.
  // - You can load multiple fonts and use ImGui::PushFont()/PopFont() to select
  // them.
  // - If a file cannot be loaded, AddFont functions will return a nullptr.
  // Please handle those errors in your code (e.g. use an assertion, display an
  // error and quit).
  // - Read 'docs/FONTS.md' for more instructions and details.
  // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use FreeType
  // for higher quality font rendering.
  // - Remember that in C/C++ if you want to include a backslash \ in a string
  // literal you need to write a double backslash \\ !
  // io.Fonts->AddFontDefaultVector();
  // io.Fonts->AddFontDefaultBitmap();
  // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
  // ImFont* font =
  // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
  // IM_ASSERT(font != nullptr);

  // Our state
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

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

  // Id del usuario a buscar en tab 2 y 3
  uint32_t user_to_find = 0;
  static int g_selected_user_id = 100;

  // Main loop
  bool done = false;
  while (!done) {
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
    // tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to
    // your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
    // data to your main application, or clear/overwrite your copy of the
    // keyboard data. Generally you may always pass all inputs to dear imgui,
    // and hide them from your application based on those two flags. [If using
    // SDL_MAIN_USE_CALLBACKS: call ImGui_ImplSDL3_ProcessEvent() from your
    // SDL_AppEvent() function]
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL3_ProcessEvent(&event);
      if (event.type == SDL_EVENT_QUIT)
        done = true;
      if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
          event.window.windowID == SDL_GetWindowID(window))
        done = true;
    }

    // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your
    // SDL_AppIterate() function]
    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
      SDL_Delay(10);
      continue;
    }

    // Start the Dear ImGui frame
    ImGui_ImplSDLGPU3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // Stuff for making the ImGui window the same size as the system window
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGuiWindowFlags imgui_window_flags = 0;
    imgui_window_flags |= ImGuiWindowFlags_NoTitleBar;
    imgui_window_flags |= ImGuiWindowFlags_NoMove;
    imgui_window_flags |= ImGuiWindowFlags_NoResize;

    ImGui::Begin("imgui window", NULL, imgui_window_flags);
    // Finalmente, el código para la GUI.
    if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None)) {
      // Tab 1
      if (ImGui::BeginTabItem("Lista de jugadores")) {
        ImGui::BeginTable("UsersTable", 4,
                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                              ImGuiTableFlags_Resizable |
                              ImGuiTableFlags_ScrollY |
                              ImGuiTableFlags_SizingStretchProp,
                          ImVec2(0.0f, 800.0f));
        ImGui::TableSetupColumn("User ID");
        ImGui::TableSetupColumn("Username");
        ImGui::TableSetupColumn("PP");
        ImGui::TableSetupColumn("Playtime");
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        static int page = 0;
        int max_entries = std::min((page + 1) * 40, (int)user_ids.size());
        bool open_popup = false;

        for (int i = page * 40; i < max_entries; i++) {
          const User &user = users[std::get<0>(user_ids[i])];

          ImGui::TableNextRow();
          ImGui::TableSetColumnIndex(0);

          ImGui::PushID(user.get_user_id());

          char user_id_text[32];
          snprintf(user_id_text, sizeof(user_id_text), "%u",
                   user.get_user_id());

          if (ImGui::Selectable(user_id_text, false,
                                ImGuiSelectableFlags_SpanAllColumns)) {
            g_selected_user_id = user.get_user_id();
            open_popup = true;
            ImGui::OpenPopup("ConfirmViewUserInfo");
          }

          ImGui::PopID();

          ImGui::TableSetColumnIndex(1);
          ImGui::TextUnformatted(user.get_username().c_str());

          ImGui::TableSetColumnIndex(2);
          ImGui::Text("%.2f", user.get_pp_value());

          ImGui::TableSetColumnIndex(3);
          const uint32_t hours = user.get_playtime() / 3600;
          const uint32_t minutes = (user.get_playtime() % 3600) / 60;
          ImGui::Text("%uh %02um", hours, minutes);
        }
        ImGui::EndTable();

        if (open_popup) {
          ImGui::OpenPopup("ConfirmViewUserInfo");
        }

        if (ImGui::BeginPopup("ConfirmViewUserInfo")) {
          ImGui::Text("¿Quieres ver la información detallada del jugador?");
          ImGui::Spacing();
          if (ImGui::Button("Si (copia el User Id a la pestaña 2)")) {
            user_to_find = g_selected_user_id;
            ImGui::CloseCurrentPopup();
          }
          ImGui::SameLine();

          if (ImGui::Button("No, permanecer aquí")) {
            ImGui::CloseCurrentPopup();
          }

          ImGui::EndPopup();
        }

        ImGui::BeginDisabled(page == 0);
        if (ImGui::Button("Página anterior")) {
          if (page > 0) {
            page--;
          }
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::Text("Página %d", page + 1);
        ImGui::SameLine();
        if (ImGui::Button("Página siguiente")) {
          page++;
        }
        ImGui::EndTabItem();
      }
      // Tab 2
      if (ImGui::BeginTabItem("Información de jugador")) {
        ImGui::Text("User id:");
        ImGui::SameLine();
        ImGui::InputScalar("##user_id", ImGuiDataType_U32, &user_to_find);
        if (user_to_find != 0) {
          try {
            auto found_user = osumm.search_user(user_to_find);
            show_user_info(found_user, "UserProfile");
          } catch (const std::out_of_range &ex) {
            std::stringstream error_msg;
            error_msg << "El usuario con ID: " << user_to_find << " no existe";
            ImGui::Text("%s", error_msg.str().c_str());
          }
        }

        ImGui::EndTabItem();
      }
      // Tab 3
      if (ImGui::BeginTabItem("Encontrar oponente")) {
        ImGui::Text("User id:");
        ImGui::SameLine();
        ImGui::InputScalar("##user_id", ImGuiDataType_U32, &user_to_find);
        if (user_to_find != 0) {

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
              User rival_user =
                  osumm.search_user(std::get<0>(user_ids[rival_index]));

              ImGui::SetWindowFontScale(2.0f);
              ImGui::Text("Usuario Buscado:");
              ImGui::SetWindowFontScale(1.0f);
              show_user_info(found_user, "FirstUser");
              ImGui::Separator();
              ImGui::SetWindowFontScale(2.0f);
              ImGui::Text("Rival seleccionado:");
              ImGui::SetWindowFontScale(1.0f);
              show_user_info(rival_user, "SecondUser");
            } catch (const std::out_of_range &ex) {
              std::stringstream error_msg;
              error_msg << "No deberia existir este error";
              ImGui::Text("%s", error_msg.str().c_str());
            }
          } catch (const std::out_of_range &ex) {
            std::stringstream error_msg;
            error_msg << "El usuario con ID: " << user_to_find << " no existe";
            ImGui::Text("%s", error_msg.str().c_str());
          }
        }

        ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
    }
    ImGui::End();

    // Rendering
    ImGui::Render();
    ImDrawData *draw_data = ImGui::GetDrawData();
    const bool is_minimized =
        (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);

    SDL_GPUCommandBuffer *command_buffer =
        SDL_AcquireGPUCommandBuffer(gpu_device); // Acquire a GPU command buffer

    SDL_GPUTexture *swapchain_texture;
    SDL_WaitAndAcquireGPUSwapchainTexture(
        command_buffer, window, &swapchain_texture, nullptr,
        nullptr); // Acquire a swapchain texture

    if (swapchain_texture != nullptr && !is_minimized) {
      // This is mandatory: call ImGui_ImplSDLGPU3_PrepareDrawData() to upload
      // the vertex/index buffer!
      ImGui_ImplSDLGPU3_PrepareDrawData(draw_data, command_buffer);

      // Setup and start a render pass
      SDL_GPUColorTargetInfo target_info = {};
      target_info.texture = swapchain_texture;
      target_info.clear_color = SDL_FColor{clear_color.x, clear_color.y,
                                           clear_color.z, clear_color.w};
      target_info.load_op = SDL_GPU_LOADOP_CLEAR;
      target_info.store_op = SDL_GPU_STOREOP_STORE;
      target_info.mip_level = 0;
      target_info.layer_or_depth_plane = 0;
      target_info.cycle = false;
      SDL_GPURenderPass *render_pass =
          SDL_BeginGPURenderPass(command_buffer, &target_info, 1, nullptr);

      // Render ImGui
      ImGui_ImplSDLGPU3_RenderDrawData(draw_data, command_buffer, render_pass);

      SDL_EndGPURenderPass(render_pass);
    }

    // Submit the command buffer
    SDL_SubmitGPUCommandBuffer(command_buffer);
  }

  // Cleanup
  // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your
  // SDL_AppQuit() function]
  SDL_WaitForGPUIdle(gpu_device);
  ImGui_ImplSDL3_Shutdown();
  ImGui_ImplSDLGPU3_Shutdown();
  ImGui::DestroyContext();

  SDL_ReleaseWindowFromGPUDevice(gpu_device, window);
  SDL_DestroyGPUDevice(gpu_device);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
