#include "game.hpp"

#include <spdlog/spdlog.h>

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

  m_asset_manager.load_assets("./data/assets.json");

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

void Game::run()
{
  while (!m_input_manager.should_quit())
  {
    m_clock.tick();
    m_input_manager.update();
    m_scene_manager.update();
    m_scene_manager.render();
    m_display.render();
  }
}
}  // namespace dl
