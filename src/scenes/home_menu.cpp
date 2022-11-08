#include "./home_menu.hpp"

namespace dl
{
  HomeMenu::HomeMenu()
    : Scene("scenes/home_menu.lua")
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

    if (m_input_manager->is_any_key_down())
    {
      set_scene("gameplay");
    }
  }

  void HomeMenu::render(TCOD_Console& console)
  {
    if (!has_loaded())
    {
      return;
    }

    tcod::print(console, {2, 8}, m_game_title, TCOD_white, std::nullopt);
    tcod::print(console, {2, 10}, m_instructions, TCOD_white, std::nullopt);
  }
}

