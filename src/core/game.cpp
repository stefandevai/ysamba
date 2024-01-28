#include "game.hpp"

#include <spdlog/spdlog.h>

#include "./file_manager.hpp"
#include "core/serialization.hpp"
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
    serialization::initialize_directories();

    const auto width = m_json.object["width"];
    const auto height = m_json.object["height"];
    const auto title = m_json.object["title"];

    m_display.load(width, height, title);

    const auto& inital_scene_key = m_json.object["initial_scene"].get<std::string>();

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
      spdlog::critical("Could not find scene: {}", inital_scene_key);
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
  while (!m_input_manager.should_quit())
  {
    m_clock.tick();

    // auto start_update = std::chrono::high_resolution_clock::now();

    m_input_manager.update();
    m_scene_manager.update();
    // auto stop_update = std::chrono::high_resolution_clock::now();

    // auto start_batching = std::chrono::high_resolution_clock::now();
    m_scene_manager.render();
    // auto stop_batching = std::chrono::high_resolution_clock::now();
    // auto start_render = std::chrono::high_resolution_clock::now();
    m_display.render();
    // auto stop_render = std::chrono::high_resolution_clock::now();

    // if (m_clock.frame % 20 == 0)
    // {
    //   auto duration_update = std::chrono::duration_cast<std::chrono::milliseconds>(stop_update - start_update);
    //   auto duration_render = std::chrono::duration_cast<std::chrono::milliseconds>(stop_render - start_render);
    //   auto duration_batching = std::chrono::duration_cast<std::chrono::milliseconds>(stop_batching - start_batching);
    //   spdlog::debug("Update took: {}ms", duration_update.count());
    //   spdlog::debug("Batching took: {}ms", duration_batching.count());
    //   spdlog::debug("Render took: {}ms", duration_render.count());
    // }
  }
}
}  // namespace dl
