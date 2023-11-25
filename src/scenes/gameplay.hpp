#pragma once

#include <entt/entity/registry.hpp>
#include <memory>

#include "./scene.hpp"
#include "core/input_manager.hpp"
#include "ecs/systems/game.hpp"
#include "ecs/systems/harvest.hpp"
#include "ecs/systems/inspector.hpp"
#include "ecs/systems/physics.hpp"
#include "ecs/systems/render.hpp"
#include "ecs/systems/society.hpp"
#include "world/world.hpp"

namespace dl
{
class Renderer;
class Camera;

class Gameplay : public Scene
{
 public:
  Gameplay(const std::string& scene_key, Camera& camera);

  void load() override;
  void update(const double delta, SetSceneFunction set_scene) override;
  void render(Renderer& renderer) override;

  void save_world(const std::string& file_path);
  void load_world(const std::string& file_path);

 private:
  enum State
  {
    PLAYING,
    PAUSED,
  };

  entt::registry m_registry;
  World m_world;
  std::shared_ptr<InputManager> m_input_manager = InputManager::get_instance();
  State m_current_state = State::PAUSED;
  entt::entity m_fps_text;
  double m_turn_delay = 0.0;

  GameSystem m_game_system{m_registry, m_world};
  PhysicsSystem m_physics_system{m_world};
  RenderSystem m_render_system{m_world};
  SocietySystem m_society_system{};
  HarvestSystem m_harvest_system{m_world};
  InspectorSystem m_inspector_system{m_world};

  void m_update_input(SetSceneFunction& set_scene);
  void m_select_entity(const float x, const float y);
};
}  // namespace dl
