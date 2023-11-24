#include "./gameplay.hpp"

#include <spdlog/spdlog.h>

#include <cereal/archives/binary.hpp>
#include <climits>
#include <fstream>

#include "../graphics/camera.hpp"
#include "../graphics/renderer.hpp"
#include "../world/society/society_generator.hpp"

// TEMP
#include "../ecs/components/position.hpp"
#include "../ecs/components/selectable.hpp"
#include "../ecs/components/visibility.hpp"
// TEMP

namespace dl
{
Gameplay::Gameplay(const std::string& scene_key, Camera& camera) : Scene(scene_key, camera) {}

void Gameplay::load()
{
  Scene::load();

  load_world("./world.dl");

  m_ecs.load();

  auto society_blueprint = m_world.get_society("otomi");
  auto components = SocietyGenerator::generate_members(society_blueprint);
  SocietyGenerator::place_members(components, m_world, m_camera, m_registry);

  m_has_loaded = true;
}

void Gameplay::update(const double delta, SetSceneFunction set_scene)
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
  cereal::BinaryOutputArchive archive(output_stream);
  archive(m_world);
}

void Gameplay::load_world(const std::string& file_path)
{
  std::ifstream input_stream(file_path);
  cereal::BinaryInputArchive archive(input_stream);
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
    spdlog::info("SEED: {}", m_world.get_seed());
  }
  else if (m_input_manager->poll_action("camera_move_west"))
  {
    m_camera.move(-8.f, 0.f);
  }
  else if (m_input_manager->poll_action("camera_move_east"))
  {
    m_camera.move(8.f, 0.f);
  }
  else if (m_input_manager->poll_action("camera_move_south"))
  {
    m_camera.move(0.f, 8.f);
  }
  else if (m_input_manager->poll_action("camera_move_north"))
  {
    m_camera.move(0.f, -8.f);
  }
  else if (m_input_manager->is_clicking(InputManager::MouseButton::Left))
  {
    const auto& mouse_position = m_input_manager->get_mouse_position();
    const auto& camera_position = m_camera.get_position();
    m_select_entity((mouse_position.first + camera_position.x) / 32.f,
                    (mouse_position.second + camera_position.y) / 32.f);
  }
}

void Gameplay::m_select_entity(const float x, const float y)
{
  const auto tile_x = std::floor(x);
  const auto tile_y = std::floor(y);
  const auto& entities = m_world.spatial_hash.get(tile_x, tile_y);

  for (const auto entity : entities)
  {
    if (m_registry.all_of<Selectable, Position>(entity))
    {
      auto& position = m_registry.get<Position>(entity);
      auto& selectable = m_registry.get<Selectable>(entity);

      if (x >= std::round(position.x) && x <= std::round(position.x) + 1.f && y >= std::round(position.y) &&
          y <= std::round(position.y) + 1.f)
      {
        selectable.selected = true;
        return;
      }
    }
  }
}
}  // namespace dl
