#include "./gameplay.hpp"

#include <fstream>
#include <random>
#include <climits>
#include <cereal/archives/binary.hpp>

namespace dl
{
  Gameplay::Gameplay(const std::string& scene_key)
    : Scene(scene_key)
  { }

  void Gameplay::load()
  {
    Scene::load();

    load_world("./world.dl");

    m_physics_layer.add(&m_player.body);
    m_camera.size.w = m_lua.get_variable<int>("camera_width");
    m_camera.size.h = m_lua.get_variable<int>("camera_height");
    m_ecs.load();
    m_has_loaded = true;
  }

  void Gameplay::update(const uint32_t delta, std::function<void(const std::string&)> set_scene)
  {
    (void)set_scene;

    if (!has_loaded())
    {
      return;
    }

    m_camera.update(m_player.body.position, m_world.get_tilemap_size(m_player.body.position.z));
    m_ecs.update();
    m_player.update(delta);

    if (m_player.should_advance_turn())
    {
      m_physics_layer.update(delta);
      m_world.update(delta);
    }

    if (m_input_manager->poll_action("quit"))
    {
      set_scene("home_menu");
    }
    else if (m_input_manager->poll_action("save_world"))
    {
      save_world("./world.dl");
    }
    else if (m_input_manager->poll_action("load_world"))
    {
      load_world("./world.dl");
    }
    else if (m_input_manager->poll_action("display_seed"))
    {
      std::cout << "SEED: " << m_world.get_seed() << '\n';
    }
  }

  void Gameplay::render(tcod::Context& context, TCOD_Console& console)
  {
    if (!has_loaded())
    {
      return;
    }

    console.clear();
    m_world.render(console, m_camera);
    m_ecs.render(console);
    m_player.render(console, m_camera);
    context.present(console);
  }

  void Gameplay::save_world(const std::string& file_path)
  {
    std::ofstream output_stream(file_path);
    cereal::BinaryOutputArchive archive (output_stream);
    archive(m_world);
  }

  void Gameplay::load_world(const std::string& file_path)
  {
    std::ifstream input_stream(file_path);
    cereal::BinaryInputArchive archive (input_stream);
    archive(m_world);
    m_has_loaded = true;
  }
}
