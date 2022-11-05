#include "./home_menu.hpp"

namespace dl
{
  HomeMenu::HomeMenu()
    : Scene("scenes/home_menu.lua")
  { }

  HomeMenu::~HomeMenu() { }

  void HomeMenu::load()
  {
    Scene::load();
    m_game_title = m_lua.get_variable<std::string>("game_title");
    m_instructions = m_lua.get_variable<std::string>("instructions");
    m_has_loaded = true;
  }

  void HomeMenu::update(const uint32_t delta, std::function<void(const std::string&)> set_scene)
  {
    if (m_input_manager->is_any_key_down())
    {
      set_scene("gameplay");
    }
  }

  void HomeMenu::render(TCOD_Console& console)
  {
    tcod::print(console, {0, 0}, m_game_title, TCOD_white, std::nullopt);
    tcod::print(console, {0, 2}, m_instructions, TCOD_white, std::nullopt);
  }
}

