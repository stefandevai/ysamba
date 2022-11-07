#pragma once

#include <memory>
#include <libtcod.hpp>
#include "./input_manager.hpp"
#include "./scene_manager.hpp"
#include "../player/player.hpp"

namespace dl
{
  class Game
  {
    public:
      Game();

      void load(int argc, char* argv[]);
      void run();

    private:
      tcod::Console m_console;
      tcod::Context m_context;
      Player m_player;
      std::shared_ptr<InputManager> m_input_manager = InputManager::get_instance();
      LuaAPI m_lua{"game.lua"};
      SceneManager m_scene_manager;
  };
}
