#include "./gameplay.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>

#include "core/events/camera.hpp"
#include "core/game_context.hpp"
#include "core/json.hpp"
#include "core/random.hpp"
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

namespace dl
{
Gameplay::Gameplay(GameContext& game_context) : Scene("gameplay", game_context) {}

void Gameplay::load()
{
  using namespace entt::literals;

  Scene::load();

  m_renderer.add_batch("world"_hs, "default"_hs);

  const auto default_zoom = m_json.object["default_zoom"].get<float>();

  m_camera.set_tile_size(m_world.get_tile_size());
  m_camera.set_zoom(default_zoom);

  m_camera.set_event_emitter(&m_event_emitter);
  m_event_emitter.on<CameraMovedEvent>([this](const CameraMovedEvent& event, EventEmitter& emitter) {
    (void)emitter;
    m_world.chunk_manager.update({event.position_in_tiles.x, event.position_in_tiles.y, 0});
  });
  m_event_emitter.on<CameraZoomedEvent>([this](const CameraZoomedEvent& event, EventEmitter& emitter) {
    (void)emitter;
    m_world.chunk_manager.set_frustum(event.frustum);
  });

#ifndef DL_BUILD_DEBUG_TOOLS
  load_game();
#else
  if (m_game_context.world_metadata.id.empty())
  {
    load_default_game();
  }
  else
  {
    load_game();
  }

  auto& debug_tools = DebugTools::get_instance();
  debug_tools.init_general_info(m_game_context);
  debug_tools.init_camera_inspector(m_camera);
  debug_tools.init_world_generation(m_world.chunk_manager);
  /* debug_tools.init_chunk_debugger(*this); */
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

  if (m_input_manager.is_context("gameplay"_hs))
  {
    if (m_update_input())
    {
      return;
    }
  }

  const auto delta = m_game_context.clock->delta;

  m_camera.update(delta);

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

  m_renderer.push_matrix("world"_hs, m_camera.view_matrix);
  m_render_system.render(m_registry, m_camera);
  m_ui_manager.render();
  m_renderer.render(m_camera);
  m_renderer.pop_matrix("world"_hs);
}

void Gameplay::save_game() { serialization::save_game(m_world, m_game_context.world_metadata, m_registry); }

void Gameplay::load_game()
{
  m_world.chunk_manager.load_initial_chunks(m_camera.center_in_tiles);

  m_registry.clear();
  serialization::load_game(m_world, m_game_context.world_metadata, m_registry);

  if (!m_world.has_initialized)
  {
    m_world.initialize(m_registry, m_camera);
    serialization::save_game(m_world, m_game_context.world_metadata, m_registry);
  }

  m_has_loaded = true;
}

void Gameplay::load_default_game()
{
  m_world.chunk_manager.mode = ChunkManager::Mode::NoLoadingOrSaving;

  m_camera.update_dirty();

  m_game_context.world_metadata.id = "test";
  m_game_context.world_metadata.seed = 100;
  m_game_context.world_metadata.world_size = {256, 256, 10};

  m_world.chunk_manager.load_initial_chunks(m_camera.center_in_tiles);

  m_world.generate_societies();
  auto society_blueprint = m_world.get_society("otomi"_hs);
  auto components = SocietyGenerator::generate_members(society_blueprint);
  SocietyGenerator::place_members(components, m_world, m_camera, m_registry);
  m_world.has_initialized = true;
  m_has_loaded = true;
}

bool Gameplay::m_update_input()
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
    const auto mouse_tile = m_world.mouse_to_world(m_camera);

    m_world.create_item(m_registry, id, mouse_tile.x, mouse_tile.y, mouse_tile.z);
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
#ifdef DL_BUILD_DEBUG_TOOLS
  else if (m_input_manager.poll_action("regenerate"_hs))
  {
    m_world.chunk_manager.regenerate_chunks();
  }
#endif

  return will_quit;
}
}  // namespace dl
