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

    m_world.generate(100, 100);
    m_physics_layer.add(&m_player.body);
    m_has_loaded = true;
  }

  void Gameplay::update(const uint32_t delta, std::function<void(const std::string&)> set_scene)
  {
    if (!has_loaded())
    {
      return;
    }

    m_player.update(delta);

    if (m_player.should_advance_turn())
    {
      m_physics_layer.update(delta);
      m_world.update(delta);
    }
  }

  void Gameplay::render(TCOD_Console& console)
  {
    if (!has_loaded())
    {
      return;
    }

    m_world.render(console);
    m_player.render(console);
  }
}

