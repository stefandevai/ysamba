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
    m_camera.size.w = m_lua.get_variable<int>("camera_width");
    m_camera.size.h = m_lua.get_variable<int>("camera_height");
    m_has_loaded = true;
  }

  void Gameplay::update(const uint32_t delta, std::function<void(const std::string&)> set_scene)
  {
    if (!has_loaded())
    {
      return;
    }

    m_player.update(delta);
    m_camera.update(m_player.body.position, m_world.get_tilemap_size(m_player.body.position.z));

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

    m_world.render(console, m_camera);
    m_player.render(console, m_camera);
  }
}

