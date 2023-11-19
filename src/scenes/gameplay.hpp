#pragma once

#include <entt/entity/registry.hpp>
#include <memory>

#include "../core/input_manager.hpp"
#include "../ecs/ecs.hpp"
#include "../world/world.hpp"
#include "./scene.hpp"

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
  ECS m_ecs{m_registry, m_world, m_camera};
  State m_current_state = State::PAUSED;

  void m_update_input(SetSceneFunction& set_scene);
  void m_select_entity(const float x, const float y);
};
}  // namespace dl
