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
    m_world.add(&m_player.body);
    m_has_loaded = true;
  }

  void Gameplay::update(const uint32_t delta, std::function<void(const std::string&)> set_scene)
  {
    m_player.update(delta);

    if (m_player.should_advance_turn())
    {
      m_world.update(delta);
      m_tilemap.update(delta);
    }
  }

  void Gameplay::render(TCOD_Console& console)
  {
    m_tilemap.render(console);
    m_player.render(console);
  }
}

