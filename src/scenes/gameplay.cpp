#include "./gameplay.hpp"

#include <fstream>
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

    m_world.generate(100, 100);
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
      m_world.generate(100, 100);
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

  void Gameplay::screenshot(tcod::Context& context, TCOD_Console& console, const std::string& filename)
  {
    int w, h;
    auto renderer = context.get_sdl_renderer();
    SDL_GetRendererOutputSize(renderer, &w, &h);
    const auto tilemap_size_in_tiles = m_world.get_tilemap_size(m_player.body.position.z);

    // TODO: Replace hardcoded tile pixel sizes
    const int tilemap_w = tilemap_size_in_tiles.w * 16;
    const int tilemap_h = tilemap_size_in_tiles.h * 16;

    SDL_Surface *main_surface = SDL_CreateRGBSurface(0, tilemap_w, tilemap_h, 32, 0, 0, 0, 0);
    SDL_Surface *surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);

    // Save original camera position
    const auto original_camera_x = m_camera.position.x;
    const auto original_camera_y = m_camera.position.y;

    for (auto i = 0; i < tilemap_w; i += w)
    {
      for (auto j = 0; j < tilemap_h; j += h)
      {
        // TODO: Replace hardcoded tile pixel sizes
        m_camera.position.x = i / 16;
        m_camera.position.y = j / 16;

        // For some reason the first 3 / 4 screens are only saved with this warm up and delay,
        // otherwise they're black.
        for (auto k = 0; k < 24; ++k)
        {
          m_world.render(console, m_camera);
          context.present(console);
        }

        SDL_Delay(500);

        m_world.render(console, m_camera);
        context.present(console);

        SDL_RenderReadPixels(renderer, nullptr, SDL_PIXELFORMAT_ARGB8888, surface->pixels, surface->pitch);

        SDL_Rect destination_rect;
        destination_rect.x = i;
        destination_rect.y = j;
        destination_rect.w = w;
        destination_rect.h = h;

        SDL_BlitSurface(surface, nullptr, main_surface, &destination_rect);
      }
    }

    // Restore original camera position
    m_camera.position.x = original_camera_x;
    m_camera.position.y = original_camera_y;

    SDL_SaveBMP(main_surface, (filename + ".bmp").c_str());

    SDL_FreeSurface(surface);
    SDL_FreeSurface(main_surface);
  }

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
}

