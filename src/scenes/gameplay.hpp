#pragma once

#include <memory>
#include <libtcod.hpp>
#include <entt/entity/registry.hpp>
#include "./scene.hpp"
#include "../core/input_manager.hpp"
#include "../player/player.hpp"
#include "../world/world.hpp"
#include "../world/camera.hpp"
#include "../ecs/ecs.hpp"

namespace dl
{
  class Renderer;

  class Gameplay : public Scene
  {
    public:
      Gameplay(const std::string& scene_key);

      void load() override;
      void update(const uint32_t delta, std::function<void(const std::string&)> set_scene) override;
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
      Camera m_camera;
      std::shared_ptr<InputManager> m_input_manager = InputManager::get_instance();
      ECS m_ecs{m_registry, m_world, m_camera};
      State m_current_state = State::PAUSED;

      void m_update_input(std::function<void(const std::string&)>& set_scene);
  };
}
