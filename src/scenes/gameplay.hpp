#pragma once

#include <libtcod.hpp>
#include "./scene.hpp"
#include "../core/input_manager.hpp"
#include "../player/player.hpp"
#include "../tilemap/tilemap.hpp"
#include "../physics/world.hpp"

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
      Tilemap m_tilemap;
      World m_world {m_tilemap};
  };
}
