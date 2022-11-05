#include "./gameplay.hpp"

namespace dl
{
  Gameplay::Gameplay()
    : Scene("scenes/gameplay.lua")
  { }

  Gameplay::~Gameplay() { }

  void Gameplay::load()
  {
    Scene::load();
    m_has_loaded = true;
  }

  void Gameplay::update(const uint32_t delta, std::function<void(const std::string&)> set_scene)
  {
    m_player.update(delta);
  }

  void Gameplay::render(TCOD_Console& console)
  {
    m_player.render(console);
  }
}

