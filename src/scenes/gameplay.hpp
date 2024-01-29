#pragma once

#include <entt/entity/registry.hpp>
#include <memory>

#include "./scene.hpp"
#include "core/input_manager.hpp"
#include "ecs/systems/action.hpp"
#include "ecs/systems/drop.hpp"
#include "ecs/systems/game.hpp"
#include "ecs/systems/inspector.hpp"
#include "ecs/systems/inventory.hpp"
#include "ecs/systems/job.hpp"
#include "ecs/systems/physics.hpp"
#include "ecs/systems/pickup.hpp"
#include "ecs/systems/render.hpp"
#include "ecs/systems/society.hpp"
#include "ecs/systems/walk.hpp"
#include "ecs/systems/wear.hpp"
#include "ecs/systems/wield.hpp"
#include "graphics/batch.hpp"
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

 private:
  friend class ChunkDebugger;

  enum State
  {
    PLAYING,
    PAUSED,
  };

  entt::registry m_registry;
  World m_world{m_game_context};
  InputManager& m_input_manager = InputManager::get_instance();
  State m_current_state = State::PAUSED;
  double m_turn_delay = 0.0;

  GameSystem m_game_system{m_registry, m_world};
  PhysicsSystem m_physics_system{m_world};
  RenderSystem m_render_system{m_renderer, m_world};
  SocietySystem m_society_system{};
  InspectorSystem m_inspector_system{m_world, m_ui_manager};
  ActionSystem m_action_system{m_world, m_ui_manager};
  PickupSystem m_pickup_system{m_world};
  WalkSystem m_walk_system{m_world};
  JobSystem m_job_system{m_world};
  InventorySystem m_inventory_system{m_world, m_ui_manager};
  WearSystem m_wear_system{m_world};
  WieldSystem m_wield_system{m_world};
  DropSystem m_drop_system{m_world, m_ui_manager};

  bool m_update_input();
};
}  // namespace dl
