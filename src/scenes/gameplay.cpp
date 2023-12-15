#include "./gameplay.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "core/game_context.hpp"
#include "core/json.hpp"
#include "core/scene_manager.hpp"
#include "core/serialization.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/visibility.hpp"
#include "graphics/camera.hpp"
#include "graphics/text.hpp"
#include "ui/compositions/debug_info.hpp"
#include "world/society/society_generator.hpp"

namespace dl
{
Gameplay::Gameplay(GameContext& game_context) : Scene("gameplay", game_context) {}

void Gameplay::load()
{
  using namespace entt::literals;

  Scene::load();

  m_renderer.add_batch("world"_hs, "default");

  m_world.load("./data/world/test_map.json");
  /* load_game(); */

  m_camera.set_tile_size(m_world.get_tile_size());
  m_debug_info = m_ui_manager.emplace<ui::DebugInfo>();

  auto society_blueprint = m_world.get_society("otomi"_hs);
  auto components = SocietyGenerator::generate_members(society_blueprint);
  SocietyGenerator::place_members(components, m_world, m_camera, m_registry);

  m_has_loaded = true;
}

double delay = 0.0;

void Gameplay::update()
{
  using namespace entt::literals;

  if (!has_loaded())
  {
    return;
  }

  const auto& current_context = m_input_manager.get_current_context();

  if (current_context != nullptr && current_context->key == "gameplay"_hs)
  {
    const auto will_quit = m_update_input(m_game_context);

    if (will_quit)
    {
      return;
    }
  }

  const auto delta = m_game_context.clock->delta;

  if (m_current_state == State::PLAYING)
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
      m_walk_system.update(m_registry);
      m_society_system.update(m_registry, delta);
      m_job_system.update(m_registry, delta);
    }
  }

  m_action_system.update(m_registry, m_camera);
  m_pickup_system.update(m_registry);
  m_wear_system.update(m_registry);
  m_inspector_system.update(m_registry, m_camera);
  m_inventory_system.update(m_registry);

  m_ui_manager.update();

  if (delay <= 0.0)
  {
    m_debug_info->set_content("FPS: " + std::to_string(1.0 / delta));
    delay = 0.8;
  }
  else
  {
    delay -= delta;
  }
}

void Gameplay::render()
{
  using namespace entt::literals;

  if (!has_loaded())
  {
    return;
  }

  m_renderer.push_matrix("world"_hs, m_camera.get_view_matrix());
  m_render_system.render(m_registry, m_renderer, m_camera);
  m_renderer.pop_matrix("world"_hs);

  m_ui_manager.render();
}

void Gameplay::save_game() { serialization::save_game(m_world, m_registry); }

void Gameplay::load_game()
{
  m_registry.clear();
  serialization::load_game(m_world, m_registry);
  m_has_loaded = true;
}

bool Gameplay::m_update_input(GameContext& m_game_context)
{
  using namespace entt::literals;

  bool will_quit = false;

  if (m_input_manager.poll_action("quit"_hs))
  {
    m_game_context.scene_manager->pop_scene();
    will_quit = true;
  }
  else if (m_input_manager.poll_action("toggle_pause"_hs))
  {
    if (m_current_state == State::PLAYING)
    {
      m_current_state = State::PAUSED;
    }
    else
    {
      m_current_state = State::PLAYING;
    }
  }
  else if (m_input_manager.poll_action("save_game"_hs))
  {
    save_game();
  }
  else if (m_input_manager.poll_action("load_game"_hs))
  {
    load_game();
  }
  else if (m_input_manager.poll_action("add_item"_hs))
  {
    JSON json{"./data/debug/item.json"};
    const auto id = json.object["id"].get<uint32_t>();

    const auto& mouse_position = m_input_manager.get_mouse_position();
    const auto& camera_position = m_camera.get_position();
    const auto& tile_size = m_camera.get_tile_size();

    const int tile_x = (mouse_position.x + camera_position.x) / tile_size.x;
    const int tile_y = (mouse_position.y + camera_position.y) / tile_size.y;

    m_world.create_item(m_registry, id, tile_x, tile_y, 1);
  }
  else if (m_input_manager.poll_action("display_seed"_hs))
  {
    spdlog::info("SEED: {}", m_world.get_seed());
  }
  else if (m_input_manager.poll_action("camera_move_west"_hs))
  {
    m_camera.move({-8., 0., 0.});
  }
  else if (m_input_manager.poll_action("camera_move_east"_hs))
  {
    m_camera.move({8., 0., 0.});
  }
  else if (m_input_manager.poll_action("camera_move_south"_hs))
  {
    m_camera.move({0., 8., 0.});
  }
  else if (m_input_manager.poll_action("camera_move_north"_hs))
  {
    m_camera.move({0., -8., 0.});
  }

  return will_quit;
}
}  // namespace dl
