#pragma once

#include <entt/entity/entity.hpp>
#include <memory>

#include "./scene.hpp"
#include "core/events/emitter.hpp"
#include "core/input_manager.hpp"
#include "ecs/systems/action.hpp"
#include "ecs/systems/drop.hpp"
#include "ecs/systems/game.hpp"
#include "ecs/systems/inspector.hpp"
#include "ecs/systems/inventory.hpp"
#include "ecs/systems/job.hpp"
#include "ecs/systems/physics.hpp"
#include "ecs/systems/pickup.hpp"
#include "ecs/systems/player_controls.hpp"
#include "ecs/systems/render.hpp"
// #include "ecs/systems/society.hpp"
#include "ecs/systems/walk.hpp"
#include "ecs/systems/wear.hpp"
#include "ecs/systems/wield.hpp"
#include "world/world.hpp"

namespace dl
{
struct GameContext;

class Gameplay : public Scene
{
 public:
  Gameplay(GameContext& game_context);

  void load() override;
  void update() override;
  void render() override;

  void save_game();
  void load_game();
#ifdef DL_BUILD_DEBUG_TOOLS
  void load_default_game();
#endif

 private:
  friend class ChunkDebugger;

  enum State
  {
    PAUSED,
    PLAYING_REAL_TIME,
    PLAYING_TURN_BASED,
  };

  EventEmitter m_event_emitter{};
  World m_world{m_game_context};
  InputManager& m_input_manager = InputManager::get_instance();
  State m_current_state = State::PAUSED;
  entt::entity m_player;
  double m_turn_delay = 0.0;

  GameSystem m_game_system{m_registry, m_world};
  PhysicsSystem m_physics_system{m_world};
  RenderSystem m_render_system{m_game_context, m_world};
  // SocietySystem m_society_system{m_world};
  InspectorSystem m_inspector_system{m_world, m_ui_manager};
  ActionSystem m_action_system{m_world, m_ui_manager, m_event_emitter};
  PickupSystem m_pickup_system{m_world};
  WalkSystem m_walk_system{m_world, m_registry};
  JobSystem m_job_system{m_world};
  InventorySystem m_inventory_system{m_world, m_ui_manager};
  WearSystem m_wear_system{m_world};
  WieldSystem m_wield_system{m_world};
  DropSystem m_drop_system{m_world, m_ui_manager};
  PlayerControlsSystem m_player_controls_system{m_event_emitter};

  bool m_update_paused();
  bool m_update_real_time();
  bool m_update_turn_based();

  void m_update_turn_systems();
  void m_update_action_systems();
  void m_update_all_systems();
  bool m_update_input_real_time();
  bool m_update_input_turn_based();

  void m_enter_turn_based(entt::entity entity);
  void m_leave_turn_based();
};
}  // namespace dl
