#include "game.hpp"

#include <spdlog/spdlog.h>

#include "./file_manager.hpp"
#include "graphics/texture.hpp"
#include "scenes/gameplay.hpp"
#include "scenes/home_menu.hpp"
#include "scenes/world_creation.hpp"

namespace dl
{
Game::Game() {}

void Game::load()
{
  spdlog::set_level(spdlog::level::debug);

  try
  {
    const auto width = m_json.object["width"];
    const auto height = m_json.object["height"];
    const auto title = m_json.object["title"];

    m_display.load(width, height, title);

    const auto& world_texture = m_asset_manager.get<Texture>("spritesheet-tileset");

    assert(world_texture != nullptr && "World texture is not loaded in order to get tile size");

    const auto& display_size = m_display.get_size();
    m_camera.set_size({static_cast<double>(display_size.x), static_cast<double>(display_size.y)});
    m_camera.set_tile_size({world_texture->get_frame_width(), world_texture->get_frame_height()});

    m_renderer.add_layer("world", "world");
    m_renderer.add_layer("quad", "quad", Renderer::LayerType::Quad, true, 1);
    m_renderer.add_layer("gui", "gui", Renderer::LayerType::Sprite, true, 2);
    m_renderer.add_layer("text", "text", Renderer::LayerType::Sprite, true, 2);

    const auto& inital_scene_key = m_json.object["initial_scene"];

    if (inital_scene_key == "home_menu")
    {
      m_scene_manager.push_scene<HomeMenu>(m_context);
    }
    else if (inital_scene_key == "world_creation")
    {
      m_scene_manager.push_scene<HomeMenu>(m_context);
      m_scene_manager.push_scene<WorldCreation>(m_context);
    }
    else if (inital_scene_key == "gameplay")
    {
      m_scene_manager.push_scene<HomeMenu>(m_context);
      m_scene_manager.push_scene<Gameplay>(m_context);
    }
    else
    {
      spdlog::critical("Could not find scene: ", inital_scene_key);
    }
  }
  catch (const std::exception& exc)
  {
    spdlog::critical("{}", exc.what());
    throw;
  }
}

void Game::run()
{
  while (!m_input_manager->should_quit())
  {
    m_clock.tick();

    m_input_manager->update();
    m_scene_manager.update(m_context);

    m_display.clear();
    m_scene_manager.render(m_renderer);
    m_renderer.render(m_camera);
    m_display.render();

    if (m_input_manager->window_size_changed())
    {
      m_handle_window_size_change();
    }
  }
}

void Game::m_handle_window_size_change()
{
  const auto& display_size = m_display.get_size();

  m_display.update_viewport();
  m_camera.set_size({static_cast<double>(display_size.x), static_cast<double>(display_size.y)});
  m_input_manager->set_window_size_changed(false);
}
}  // namespace dl
