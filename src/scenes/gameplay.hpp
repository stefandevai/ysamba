#pragma once

#include <libtcod.hpp>
#include "./scene.hpp"
#include "../core/input_manager.hpp"
#include "../player/player.hpp"
#include "../world/world.hpp"
#include "../physics/physics_layer.hpp"

namespace dl
{
  class Gameplay : public Scene
  {
    public:
      Gameplay();
      ~Gameplay();

      void load() override;
      void update(const uint32_t delta, std::function<void(const std::string&)> set_scene) override;
      void render(TCOD_Console& console) override;

    private:
      Player m_player;
      World m_world;
      PhysicsLayer m_physics_layer {m_world};
  };
}
