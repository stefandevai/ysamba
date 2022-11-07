#pragma once

#include <memory>
#include <libtcod.hpp>
#include "./scene.hpp"
#include "../core/input_manager.hpp"
#include "../player/player.hpp"
#include "../world/world.hpp"
#include "../world/camera.hpp"
#include "../physics/physics_layer.hpp"

namespace dl
{
  class Gameplay : public Scene
  {
    public:
      Gameplay();

      void load() override;
      void update(const uint32_t delta, std::function<void(const std::string&)> set_scene) override;
      void render(TCOD_Console& console) override;

      void save_world(const std::string& file_path);
      void load_world(const std::string& file_path);

    private:
      Player m_player;
      World m_world;
      Camera m_camera;
      PhysicsLayer m_physics_layer {m_world};
      std::shared_ptr<InputManager> m_input_manager = InputManager::get_instance();
  };
}
