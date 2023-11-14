#include "./world_creation.hpp"

#include <fstream>
#include <random>
#include <climits>
#include <SDL.h>
#include <cereal/archives/binary.hpp>

namespace dl
{
  WorldCreation::WorldCreation(const std::string& scene_key)
    : Scene(scene_key)
  { }

  void WorldCreation::load()
  {
    Scene::load();

    const auto seed = m_lua.get_optional_variable<int>("seed");

    if (seed)
    {
      m_generate_map(seed.value());
    }
    else
    {
      m_generate_map();
    }
    m_should_update_world_surface = true;
    m_has_loaded = true;
  }

  void WorldCreation::update(const uint32_t delta, std::function<void(const std::string&)> set_scene)
  {
    (void)set_scene;

    if (!has_loaded())
    {
      return;
    }

    if (m_input_manager->poll_action("quit"))
    {
      set_scene("home_menu");
    }
    if (m_input_manager->is_key_down(SDL_SCANCODE_G))
    {
      m_generate_map();
      m_should_update_world_surface = true;
    }
    else if (m_input_manager->is_key_down(SDL_SCANCODE_R))
    {
      m_generate_map(m_seed);
      m_should_update_world_surface = true;
    }
    else if (m_input_manager->is_key_down(SDL_SCANCODE_S))
    {
      save_world("./world.dl");
    }
    else if (m_input_manager->is_key_down(SDL_SCANCODE_C))
    {
      load_world("./world.dl");
      m_should_update_world_surface = true;
    }
    else if (m_input_manager->is_key_down(SDL_SCANCODE_X))
    {
      std::cout << "SEED: " << m_world.get_seed() << '\n';
    }
  }

  void WorldCreation::render(tcod::Context& context, TCOD_Console& console)
  {
    if (!has_loaded())
    {
      return;
    }

    if (m_should_update_world_surface)
    {
      m_create_world_surface(context);
      m_should_update_world_surface = false;
    }

    auto renderer = context.get_sdl_renderer();
    auto texture = SDL_CreateTextureFromSurface(renderer, m_surface);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
  }

  void WorldCreation::screenshot(tcod::Context& context, TCOD_Console& console, const std::string& filename)
  {
    (void)context;
    (void)console;

    const auto tilemap_size = m_world.get_tilemap_size(0);
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

        const auto tile = m_world.get(i, j, 0);
        const auto color = SDL_MapRGB(surface->format, tile.r, tile.g, tile.b);

        SDL_FillRect(surface, &rect, color);
      }
    }

    SDL_SaveBMP(surface, (filename + ".bmp").c_str());

    SDL_FreeSurface(surface);
  }

  void WorldCreation::save_world(const std::string& file_path)
  {
    std::ofstream output_stream(file_path);
    cereal::BinaryOutputArchive archive (output_stream);
    archive(m_world);
  }

  void WorldCreation::load_world(const std::string& file_path)
  {
    std::ifstream input_stream(file_path);
    cereal::BinaryInputArchive archive (input_stream);
    archive(m_world);
    m_has_loaded = true;
  }

  void WorldCreation::m_generate_map(const int seed)
  {
    const int map_width = m_lua.get_variable<int>("map_width");
    const int map_height = m_lua.get_variable<int>("map_height");

    if (seed != 0)
    {
      m_seed = seed;
    }
    else
    {
      std::random_device dev;
      std::mt19937 rng(dev());
      std::uniform_int_distribution<int> dist(1, INT_MAX);

      m_seed = dist(rng);
    }

    m_world.generate(map_width, map_height, m_seed);
  }

  void WorldCreation::m_create_world_surface(tcod::Context& context)
  {
    int window_width, window_height;
    auto window = context.get_sdl_window();
    const auto tilemap_size = m_world.get_tilemap_size(0);

    SDL_GetWindowSize(window, &window_width, &window_height);

    if (m_surface != nullptr)
    {
      SDL_FreeSurface(m_surface);
    }

    m_surface = SDL_CreateRGBSurface(0, window_width, window_height, 32, 0, 0, 0, 0);

    for (auto i = 0; i < tilemap_size.w; ++i)
    {
      for (auto j = 0; j < tilemap_size.h; ++j)
      {
        SDL_Rect rect;
        rect.x = i;
        rect.y = j;
        rect.w = 1;
        rect.h = 1;

        const auto tile = m_world.get(i, j, 0);
        const auto color = SDL_MapRGB(m_surface->format, tile.r, tile.g, tile.b);

        SDL_FillRect(m_surface, &rect, color);
      }
    }
  }
}

