#pragma once

#include <memory>
#include <libtcod.hpp>
#include "./display.hpp"
#include "./input_manager.hpp"
#include "./scene_manager.hpp"
/* #include "../player/player.hpp" */
#include "../graphics/asset_manager.hpp"

namespace dl
{
  class Game
  {
    public:
      Game();

      void load(int argc, char* argv[]);
      void run();

    private:
      Display m_display{800, 600, "Dialetics"};
      AssetManager m_asset_manager{"./data/assets.json"};
      /* tcod::Console m_console; */
      /* tcod::Context m_context; */
      /* Player m_player; */
      std::shared_ptr<InputManager> m_input_manager = InputManager::get_instance();
      LuaAPI m_lua{"game.lua"};
      SceneManager m_scene_manager;
  };
}
