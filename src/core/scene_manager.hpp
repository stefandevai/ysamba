#pragma once

#include <map>
#include <memory>
#include "../scenes/scene.hpp"
#include "./lua_api.hpp"

namespace dl
{
  enum SceneType
  {
    HOME_MENU,
    WORLD_CREATION,
    GAMEPLAY,
  };

  class Renderer;

  class SceneManager
  {
    public:
      SceneManager();

      static void set_scene(const std::string& key);
      void update(const uint32_t delta);
      void render(Renderer& renderer);
      /* void screenshot(tcod::Context& context, TCOD_Console& console, const std::string& filename); */

      inline std::shared_ptr<Scene> get_current_scene() const { return m_current_scene; };

    private:
      LuaAPI m_lua{"scenes/scenes.lua"};
      std::string m_inital_scene_key;
      static std::shared_ptr<Scene> m_current_scene;
      static std::map<std::string, SceneType> m_scenes_data;

      void m_load();
  };
}
