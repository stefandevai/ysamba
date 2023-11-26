#pragma once

#include <filesystem>
#include <string>

#include "core/lua_api.hpp"

namespace dl
{
class Renderer;
struct GameContext;

class Scene
{
 public:
  Scene(const std::string& scene_key, GameContext& game_context);
  virtual ~Scene();

  virtual void load();
  virtual void update(GameContext& game_context) = 0;
  virtual void render(Renderer& renderer) = 0;
  inline bool has_loaded() const { return m_has_loaded; }
  const std::string& get_key() const { return m_scene_key; }

 protected:
  const std::string m_scene_key;
  const std::filesystem::path m_scene_dir;
  GameContext& m_game_context;
  LuaAPI m_lua;
  bool m_has_loaded = false;
};
}  // namespace dl
