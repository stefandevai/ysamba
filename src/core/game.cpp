#include "game.hpp"

#include <spdlog/spdlog.h>

#include <i18n_keyval/i18n.hpp>
#include <i18n_keyval/translators/nlohmann_json.hpp>

#include "config.hpp"
#include "core/serialization.hpp"
#include "graphics/texture.hpp"
#include "scenes/gameplay.hpp"
#include "scenes/home_menu.hpp"
#include "scenes/world_creation.hpp"

namespace dl
{
void Game::load()
{
  spdlog::set_level(spdlog::level::debug);

  config::load();

  i18n::set_locale("es");
  i18n::initialize_translator<i18n::translators::nlohmann_json>(config::path::translations);

  m_asset_manager.load_assets(config::path::assets);

  serialization::initialize_directories();

  m_display.load(config::display::default_width, config::display::default_height, config::display::title);

  if (config::initial_scene == "home_menu")
  {
    m_scene_manager.push_scene<HomeMenu>(m_context);
  }
  else if (config::initial_scene == "world_creation")
  {
    m_scene_manager.push_scene<HomeMenu>(m_context);
    m_scene_manager.push_scene<WorldCreation>(m_context);
  }
  else if (config::initial_scene == "gameplay")
  {
    m_scene_manager.push_scene<HomeMenu>(m_context);
    m_scene_manager.push_scene<Gameplay>(m_context);
  }
  else
  {
    spdlog::critical("Could not find scene: {}", config::initial_scene);
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
  }
}
}  // namespace dl
