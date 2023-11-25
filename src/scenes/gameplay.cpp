#include "./gameplay.hpp"

#include <spdlog/spdlog.h>

#include <cereal/archives/binary.hpp>
#include <climits>
#include <fstream>

#include "graphics/camera.hpp"
#include "graphics/renderer.hpp"
#include "world/society/society_generator.hpp"

// TEMP
#include "ecs/components/biology.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/selectable.hpp"
#include "ecs/components/society_agent.hpp"
#include "ecs/components/velocity.hpp"
#include "ecs/components/visibility.hpp"
#include "graphics/text.hpp"
#include "world/society/sex.hpp"
// TEMP

namespace dl
{
Gameplay::Gameplay(const std::string& scene_key, Camera& camera) : Scene(scene_key, camera) {}

void Gameplay::load()
{
  Scene::load();

  load_world("./world.dl");

  m_fps_text = m_registry.create();
  m_registry.emplace<Text>(m_fps_text, "FPS: ");
  m_registry.emplace<Position>(m_fps_text, 30, 30, 20);
  auto& text_component = m_registry.get<Text>(m_fps_text);
  text_component.set_is_static(false);

  /* auto society_blueprint = m_world.get_society("otomi"); */
  /* auto components = SocietyGenerator::generate_members(society_blueprint); */
  /* SocietyGenerator::place_members(components, m_world, m_camera, m_registry); */

  m_world.set(5, 17, 14, 0);
  /* m_world.set(5, 17, 13, 0); */
  /* m_world.set(5, 17, 15, 0); */
  /* m_world.set(5, 15, 15, 0); */
  /* m_world.set(5, 15, 13, 0); */

  const auto entity = m_registry.create();
  m_registry.emplace<SocietyAgent>(entity, 0, "otomi", "AAA", SocialClass::None, Metier::None);
  m_registry.emplace<Biology>(entity, Sex::Male, 100);
  m_registry.emplace<Position>(entity, 16, 14);
  m_registry.emplace<Visibility>(entity, "spritesheet-characters", 0, 2);
  m_registry.emplace<Selectable>(entity);

  const auto entity2 = m_registry.create();
  m_registry.emplace<SocietyAgent>(entity2, 0, "otomi", "BBB", SocialClass::None, Metier::None);
  m_registry.emplace<Biology>(entity2, Sex::Male, 100);
  m_registry.emplace<Position>(entity2, 17, 13);
  m_registry.emplace<Visibility>(entity2, "spritesheet-characters", 0, 2);
  m_registry.emplace<Selectable>(entity2);

  const auto entity3 = m_registry.create();
  m_registry.emplace<SocietyAgent>(entity3, 0, "otomi", "KKK", SocialClass::None, Metier::None);
  m_registry.emplace<Biology>(entity3, Sex::Male, 100);
  m_registry.emplace<Position>(entity3, 15, 13);
  m_registry.emplace<Visibility>(entity3, "spritesheet-characters", 0, 2);
  m_registry.emplace<Selectable>(entity3);

  m_has_loaded = true;
}

double delay = 0.0;

void Gameplay::update(const double delta, SetSceneFunction set_scene)
{
  if (!has_loaded())
  {
    return;
  }

  if (m_current_state == Gameplay::State::PLAYING)
  {
    if (m_turn_delay > 0.0)
    {
      m_turn_delay -= delta;
    }
    else
    {
      m_turn_delay = 0.5;
      m_game_system.update();
      m_physics_system.update(m_registry, delta);
      m_society_system.update(m_registry, delta);
      m_harvest_system.update(m_registry, delta);
    }
  }

  m_inspector_system.update(m_registry, m_camera);

  if (delay <= 0.0)
  {
    auto& text = m_registry.get<Text>(m_fps_text);
    text.set_text("FPS: " + std::to_string(1.0 / delta));
    delay = 0.8;
  }
  else
  {
    delay -= delta;
  }

  m_update_input(set_scene);
}

void Gameplay::render(Renderer& renderer)
{
  if (!has_loaded())
  {
    return;
  }

  m_render_system.update(m_registry, renderer, m_camera);
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
    m_camera.move({-8., 0., 0.});
  }
  else if (m_input_manager->poll_action("camera_move_east"))
  {
    m_camera.move({8., 0., 0.});
  }
  else if (m_input_manager->poll_action("camera_move_south"))
  {
    m_camera.move({0., 8., 0.});
  }
  else if (m_input_manager->poll_action("camera_move_north"))
  {
    m_camera.move({0., -8., 0.});
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
