#include "./gameplay.hpp"

#include <fstream>
#include <random>
#include <climits>
#include <cereal/archives/binary.hpp>
#include <spdlog/spdlog.h>
#include "../world/generators/society_generator.hpp"
#include "../graphics/camera.hpp"
#include "../graphics/renderer.hpp"
#include "../ecs/components/selectable.hpp"

namespace dl
{
  Gameplay::Gameplay(const std::string& scene_key, ViewCamera& camera)
    : Scene(scene_key, camera)
  { }

  void Gameplay::load()
  {
    Scene::load();

    load_world("./world.dl");

    m_ecs.load();

    const auto& society = m_world.get_society("otomi");
    const auto generator = SocietyGenerator();
    generator.generate_members(100, society, m_registry);

    m_has_loaded = true;
  }

  void Gameplay::update(const uint32_t delta, SetSceneFunction set_scene)
  {
    if (!has_loaded())
    {
      return;
    }

    if (m_current_state == Gameplay::State::PLAYING)
    {
      m_ecs.update(delta);
    }

    m_update_input(set_scene);
  }

  void Gameplay::render(Renderer& renderer)
  {
    if (!has_loaded())
    {
      return;
    }

    m_ecs.render(renderer);
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

  void Gameplay::m_update_input(SetSceneFunction& set_scene)
  {
    if (m_input_manager->poll_action("quit"))
    {
      set_scene("home_menu", m_camera);
    }
    else if (m_input_manager->poll_action("toggle_pause"))
    {
      if (m_current_state == Gameplay::State::PLAYING)
      {
        m_current_state = Gameplay::State::PAUSED;
      }
      else
      {
        m_current_state = Gameplay::State::PLAYING;
      }
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
    else if (m_input_manager->poll_action("camera_move_west"))
    {
      m_camera.move(-1.f, 0.f);
    }
    else if (m_input_manager->poll_action("camera_move_east"))
    {
      m_camera.move(1.f, 0.f);
    }
    else if (m_input_manager->poll_action("camera_move_south"))
    {
      m_camera.move(0.f, 1.f);
    }
    else if (m_input_manager->poll_action("camera_move_north"))
    {
      m_camera.move(0.f, -1.f);
    }
    else if (m_input_manager->is_clicking(InputManager::MouseButton::Left))
    {
      const auto& mouse_position = m_input_manager->get_mouse_position();
      m_select_entity(mouse_position.first, mouse_position.second);
    }
  }

  void Gameplay::m_select_entity(const int x, const int y)
  {
    const auto& entities = m_world.spatial_hash.get(x, y);

    for (const auto entity : entities)
    {
      if (m_registry.all_of<Selectable, Position>(entity))
      {
        auto& position = m_registry.get<Position>(entity);
        auto& selectable = m_registry.get<Selectable>(entity);

        if (x >= position.x && x <= position.x + 32 && y >= position.y && y <= position.y + 32)
        {
          selectable.selected = true;
        }
      }
    }
  }
}
