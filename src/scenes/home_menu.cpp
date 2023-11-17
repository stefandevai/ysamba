#include "./home_menu.hpp"

#include "../graphics/renderer.hpp"

namespace dl
{
  HomeMenu::HomeMenu(const std::string& scene_key)
    : Scene(scene_key)
  { }

  void HomeMenu::load()
  {
    Scene::load();

    m_game_title = m_lua.get_variable<std::string>("game_title");
    m_instructions = m_lua.get_variable<std::string>("instructions");
    m_has_loaded = true;
  }

  void HomeMenu::update(const uint32_t delta, std::function<void(const std::string&)> set_scene)
  {
    (void)delta;

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
      set_scene("gameplay");
    }
    else if (m_input_manager->poll_action("create_world"))
    {
      set_scene("world_creation");
    }
  }

  void HomeMenu::render(Renderer& renderer)
  {
    if (!has_loaded())
    {
      return;
    }

    /* console.clear(); */
    /* tcod::print(console, {2, 8}, m_game_title, TCOD_white, std::nullopt); */
    /* tcod::print(console, {2, 10}, m_instructions, TCOD_white, std::nullopt); */
    /* context.present(console); */
  }
}

