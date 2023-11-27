#include "./home_menu.hpp"

#include <spdlog/spdlog.h>

#include "core/game_context.hpp"
#include "core/scene_manager.hpp"
#include "scenes/gameplay.hpp"
#include "scenes/world_creation.hpp"
#include "world/society/name_generator.hpp"

namespace dl
{
HomeMenu::HomeMenu(GameContext& game_context) : Scene("home_menu", game_context) {}

void HomeMenu::load()
{
  Scene::load();

  m_renderer.add_layer("text", "text", Renderer::LayerType::Sprite, true, 2);

  const auto game_title = m_lua.get_variable<std::string>("game_title");
  const auto instructions = m_lua.get_variable<std::string>("instructions");

  m_game_title.set_text(game_title);
  m_game_title.set_typeface("font-1980");
  m_game_title.set_font_size(32);

  m_instructions.set_text(instructions);
  m_instructions.set_typeface("font-1980");
  m_instructions.set_font_size(32);
  m_has_loaded = true;
}

void HomeMenu::update()
{
  if (!has_loaded())
  {
    return;
  }

  if (m_input_manager->poll_action("quit"))
  {
    m_input_manager->quit();
  }
  else if (m_input_manager->poll_action("play"))
  {
    m_game_context.scene_manager->push_scene<Gameplay>(m_game_context);
  }
  else if (m_input_manager->poll_action("create_world"))
  {
    m_game_context.scene_manager->push_scene<WorldCreation>(m_game_context);
  }
}

void HomeMenu::render()
{
  if (!has_loaded())
  {
    return;
  }

  m_renderer.init("text");
  m_renderer.batch("text", m_game_title, 60, 60, 0);
  m_renderer.batch("text", m_instructions, 60, 108, 0);
  m_renderer.finalize("text");
}
}  // namespace dl
