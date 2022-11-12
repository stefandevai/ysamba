#include "./gameplay.hpp"

#include <fstream>
#include <random>
#include <climits>
#include <SDL.h>
#include <cereal/archives/binary.hpp>

namespace dl
{
  Gameplay::Gameplay()
    : Scene("scenes/gameplay.lua")
  { }

  void Gameplay::load()
  {
    Scene::load();

    m_generate_map();
    m_physics_layer.add(&m_player.body);
    m_camera.size.w = m_lua.get_variable<int>("camera_width");
    m_camera.size.h = m_lua.get_variable<int>("camera_height");
    m_has_loaded = true;
  }

  void Gameplay::update(const uint32_t delta, std::function<void(const std::string&)> set_scene)
  {
    (void)set_scene;

    if (!has_loaded())
    {
      return;
    }

    // If G is pressed
    if (m_input_manager->is_key_down(10))
    {
      m_generate_map();
    }
    // If R is pressed
    if (m_input_manager->is_key_down(21))
    {
      m_refresh_map();
    }
    // If S is pressed
    else if (m_input_manager->is_key_down(22))
    {
      save_world("./world.dl");
    }
    // If C is pressed
    else if (m_input_manager->is_key_down(6))
    {
      load_world("./world.dl");
    }

    m_camera.update(m_player.body.position, m_world.get_tilemap_size(m_player.body.position.z));
    m_player.update(delta);

    if (m_player.should_advance_turn())
    {
      m_physics_layer.update(delta);
      m_world.update(delta);
    }
  }

  void Gameplay::render(TCOD_Console& console)
  {
    if (!has_loaded())
    {
      return;
    }

    m_world.render(console, m_camera);
    m_player.render(console, m_camera);
  }

  void Gameplay::render_map(tcod::Context& context)
  {
    int w, h;
    auto window = context.get_sdl_window();
    const auto tilemap_size = m_world.get_tilemap_size(m_player.body.position.z);

    SDL_GetWindowSize(window, &w, &h);

    if (w != tilemap_size.w || h != tilemap_size.h)
    {
      SDL_SetWindowSize(window, tilemap_size.w, tilemap_size.h);
    }

    auto renderer = context.get_sdl_renderer();

    auto texture = SDL_CreateTextureFromSurface(renderer, m_surface);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
  }

  void Gameplay::screenshot(tcod::Context& context, TCOD_Console& console, const std::string& filename)
  {
    (void)context;
    (void)console;

    const auto tilemap_size = m_world.get_tilemap_size(m_player.body.position.z);
    SDL_Surface *surface = SDL_CreateRGBSurface(0, tilemap_size.w, tilemap_size.h, 32, 0, 0, 0, 0);

    for (auto i = 0; i < tilemap_size.w; ++i)
    {
      for (auto j = 0; j < tilemap_size.h; ++j)
      {
        SDL_Rect rect;
        rect.x = i;
        rect.y = j;
        rect.w = 1;
        rect.h = 1;

        const auto tile = m_world.get(i, j, m_player.body.position.z);
        const auto color = SDL_MapRGB(surface->format, tile.r, tile.g, tile.b);

        SDL_FillRect(surface, &rect, color);
      }
    }

    SDL_SaveBMP(surface, (filename + ".bmp").c_str());

    SDL_FreeSurface(surface);
  }

  // Take real screenshot of tiles
  /* void Gameplay::screenshot(tcod::Context& context, TCOD_Console& console, const std::string& filename) */
  /* { */
  /*   int w, h; */
  /*   auto renderer = context.get_sdl_renderer(); */
  /*   SDL_GetRendererOutputSize(renderer, &w, &h); */
  /*   const auto tilemap_size_in_tiles = m_world.get_tilemap_size(m_player.body.position.z); */

  /*   // TODO: Replace hardcoded tile pixel sizes */
  /*   const int tilemap_w = tilemap_size_in_tiles.w * 16; */
  /*   const int tilemap_h = tilemap_size_in_tiles.h * 16; */

  /*   SDL_Surface *main_surface = SDL_CreateRGBSurface(0, tilemap_w, tilemap_h, 32, 0, 0, 0, 0); */
  /*   SDL_Surface *surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0); */

  /*   // Save original camera position */
  /*   const auto original_camera_x = m_camera.position.x; */
  /*   const auto original_camera_y = m_camera.position.y; */

  /*   for (auto i = 0; i < tilemap_w; i += w) */
  /*   { */
  /*     for (auto j = 0; j < tilemap_h; j += h) */
  /*     { */
  /*       // TODO: Replace hardcoded tile pixel sizes */
  /*       m_camera.position.x = i / 16; */
  /*       m_camera.position.y = j / 16; */

  /*       // For some reason the first 3 / 4 screens are only saved with this warm up and delay, */
  /*       // otherwise they're black. */
  /*       for (auto k = 0; k < 24; ++k) */
  /*       { */
  /*         m_world.render(console, m_camera); */
  /*         context.present(console); */
  /*       } */

  /*       SDL_Delay(500); */

  /*       m_world.render(console, m_camera); */
  /*       context.present(console); */

  /*       SDL_RenderReadPixels(renderer, nullptr, SDL_PIXELFORMAT_ARGB8888, surface->pixels, surface->pitch); */

  /*       SDL_Rect destination_rect; */
  /*       destination_rect.x = i; */
  /*       destination_rect.y = j; */
  /*       destination_rect.w = w; */
  /*       destination_rect.h = h; */

  /*       SDL_BlitSurface(surface, nullptr, main_surface, &destination_rect); */
  /*     } */
  /*   } */

  /*   // Restore original camera position */
  /*   m_camera.position.x = original_camera_x; */
  /*   m_camera.position.y = original_camera_y; */

  /*   SDL_SaveBMP(main_surface, (filename + ".bmp").c_str()); */

  /*   SDL_FreeSurface(surface); */
  /*   SDL_FreeSurface(main_surface); */
  /* } */

  void Gameplay::save_world(const std::string& file_path)
  {
    std::ofstream output_stream(file_path);
    cereal::BinaryOutputArchive archive (output_stream);
    archive(m_world);
  }

  void Gameplay::load_world(const std::string& file_path)
  {
    std::ifstream input_stream(file_path);
    cereal::BinaryInputArchive archive (input_stream);
    archive(m_world);
    m_has_loaded = true;
  }

  void Gameplay::m_generate_map()
  {
    const int map_width = m_lua.get_variable<int>("map_width");
    const int map_height = m_lua.get_variable<int>("map_height");

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, INT_MAX);

    m_seed = dist(rng);
    /* m_seed = 112; */
    m_world.generate(map_width, map_height, m_seed);
    m_create_surface(map_width, map_height);
  }

  void Gameplay::m_refresh_map()
  {
    const int map_width = m_lua.get_variable<int>("map_width");
    const int map_height = m_lua.get_variable<int>("map_height");

    m_world.generate(map_width, map_height, m_seed);
    m_create_surface(map_width, map_height);
  }

  void Gameplay::m_create_surface(const int width, const int height)
  {
    if (m_surface != nullptr)
    {
      SDL_FreeSurface(m_surface);
    }

    m_surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);

    for (auto i = 0; i < width; ++i)
    {
      for (auto j = 0; j < height; ++j)
      {
        SDL_Rect rect;
        rect.x = i;
        rect.y = j;
        rect.w = 1;
        rect.h = 1;

        const auto tile = m_world.get(i, j, m_player.body.position.z);
        const auto color = SDL_MapRGB(m_surface->format, tile.r, tile.g, tile.b);

        SDL_FillRect(m_surface, &rect, color);
      }
    }
  }
}

