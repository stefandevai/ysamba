#include "./gameplay.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "core/game_context.hpp"
#include "core/json.hpp"
#include "core/scene_manager.hpp"
#include "core/serialization.hpp"
#include "definitions.hpp"
#include "ecs/components/item.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/visibility.hpp"
#include "graphics/camera.hpp"
#include "graphics/text.hpp"
#include "world/chunk_manager.hpp"
#include "world/society/society_generator.hpp"

#ifdef DL_BUILD_DEBUG_TOOLS
#include "debug/debug_tools.hpp"
#endif

// TEMP
#include "ecs/components/rectangle.hpp"
// TEMP

namespace dl
{
ChunkManager m_chunk_manager{};
std::vector<entt::entity> chunk_quads{};

Gameplay::Gameplay(GameContext& game_context) : Scene("gameplay", game_context) {}

void Gameplay::load()
{
  using namespace entt::literals;

  Scene::load();

  m_renderer.add_batch("world"_hs, "default");

  const auto default_zoom = m_json.object["default_zoom"].get<float>();

  /* m_world.generate(40, 40, 1893); */
  /* m_world.load("./data/world/test_map.json"); */
  /* load_game(); */

  m_camera.set_tile_size(m_world.get_tile_size());
  m_camera.set_zoom(default_zoom);
  /* m_camera.set_position({0.0, 0.0, 0.0}); */

  /* auto society_blueprint = m_world.get_society("otomi"_hs); */
  /* auto components = SocietyGenerator::generate_members(society_blueprint); */
  /* SocietyGenerator::place_members(components, m_world, m_camera, m_registry); */

  auto entity = m_registry.create();
  m_registry.emplace<Rectangle>(entity, 16, 16, 0xFFFFFFFF);
  m_registry.emplace<Position>(entity, 1, 35, 0);

#ifdef DL_BUILD_DEBUG_TOOLS
  auto& debug_tools = DebugTools::get_instance();
  debug_tools.init_general_info(m_game_context);
  debug_tools.init_camera_inspector(m_camera);
  /* debug_tools.init_render_editor(m_render_system); */
#endif

  m_has_loaded = true;
}

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

  m_camera.update(delta);

  const auto& mouse_position = m_input_manager.get_mouse_tile_position(m_camera);
  const auto& grid_size = m_camera.get_grid_size();

  m_chunk_manager.update({mouse_position.x, mouse_position.y, 0});

  for (const auto& chunk : m_chunk_manager.chunks)
  {
    bool found = false;

    for (const auto entity : chunk_quads)
    {
      const auto& position = m_registry.get<Position>(entity);

      if (position.x == chunk->position.x && position.y == chunk->position.y && position.z == chunk->position.z)
      {
        auto& rect = m_registry.get<Rectangle>(entity);

        if (chunk->active && rect.quad.color.int_color != 0xCC8844FF)
        {
          rect.quad.color.set(0xCC8844FF);
        }
        else if (!chunk->active && rect.quad.color.int_color != 0x4477AAFF)
        {
          rect.quad.color.set(0x4477AAFF);
        }

        found = true;
        break;
      }
    }

    if (!found)
    {
      auto color = chunk->active ? 0xCC8844FF : 0x4477AAFF;
      auto entity = m_registry.create();
      m_registry.emplace<Rectangle>(
          entity, m_chunk_manager.chunk_size.x * grid_size.x, m_chunk_manager.chunk_size.y * grid_size.y, color);
      /* m_registry.emplace<Position>(entity, chunk->position.x * grid_size.x, chunk->position.y * grid_size.x,
       * chunk->position.z * grid_size.x); */
      m_registry.emplace<Position>(entity, chunk->position.x, chunk->position.y, chunk->position.z);
      chunk_quads.push_back(entity);
    }
  }

  for (const auto entity : chunk_quads)
  {
    const auto& position = m_registry.get<Position>(entity);
    bool found = false;

    for (const auto& chunk : m_chunk_manager.chunks)
    {
      if (position.x == chunk->position.x && position.y == chunk->position.y && position.z == chunk->position.z)
      {
        found = true;
        break;
      }
    }

    if (!found)
    {
      m_registry.destroy(entity);
    }
  }

  std::erase_if(chunk_quads, [this](const auto entity) { return !m_registry.valid(entity); });

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
  m_wield_system.update(m_registry);
  m_inspector_system.update(m_registry, m_camera);
  m_inventory_system.update(m_registry);
  m_drop_system.update(m_registry, m_camera);

  m_ui_manager.update();
}

void Gameplay::render()
{
  using namespace entt::literals;

  if (!has_loaded())
  {
    return;
  }

  m_renderer.push_matrix("world"_hs, m_camera.get_view_matrix());
  m_render_system.render(m_registry, m_camera);
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

#ifdef DL_BUILD_DEBUG_TOOLS
    auto& debug_tools = DebugTools::get_instance();
    debug_tools.open = false;
#endif
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
    const auto& grid_size = m_camera.get_grid_size();

    const int grid_x = (mouse_position.x + camera_position.x) / grid_size.x;
    const int grid_y = (mouse_position.y + camera_position.y) / grid_size.y;

    m_world.create_item(m_registry, id, grid_x, grid_y, 0);
  }
  else if (m_input_manager.poll_action("display_seed"_hs))
  {
    spdlog::info("SEED: {}", m_world.get_seed());
  }
  else if (m_input_manager.poll_action("camera_move_west"_hs))
  {
    m_camera.move_in_grid({-1, 0, 0});
  }
  else if (m_input_manager.poll_action("camera_move_east"_hs))
  {
    m_camera.move_in_grid({1, 0, 0});
  }
  else if (m_input_manager.poll_action("camera_move_south"_hs))
  {
    m_camera.move_in_grid({0, 1, 0});
  }
  else if (m_input_manager.poll_action("camera_move_north"_hs))
  {
    m_camera.move_in_grid({0, -1, 0});
  }
  else if (m_input_manager.poll_action("zoom_in"_hs))
  {
    m_camera.zoom_in();
  }
  else if (m_input_manager.poll_action("zoom_out"_hs))
  {
    m_camera.zoom_out();
  }

  return will_quit;
}
}  // namespace dl
