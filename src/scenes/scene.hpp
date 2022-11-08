#pragma once

#include <libtcod.hpp>
#include <string>
#include "../core/lua_api.hpp"

namespace dl
{
  class Scene
  {
    public:
      Scene(const std::string& scene_path);
      virtual ~Scene();

      virtual void load();
      virtual void update(const uint32_t delta, std::function<void(const std::string&)> set_scene) = 0;
      virtual void render(TCOD_Console& console) = 0;
      virtual void screenshot(tcod::Context& context, TCOD_Console& console, const std::string& filename);
      inline bool has_loaded() const { return m_has_loaded; };

    protected:
      const std::string m_scene_path;
      LuaAPI m_lua;
      bool m_has_loaded = false;
  };
}
