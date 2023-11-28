#pragma once

#include <filesystem>
#include <string>

#include "core/game_context.hpp"
#include "core/lua_api.hpp"
#include "graphics/camera.hpp"
#include "graphics/renderer.hpp"

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
  virtual void update() = 0;
  virtual void render() = 0;
  void render_call() { m_renderer.render(); }
  void check_window_size();
  bool has_loaded() const { return m_has_loaded; }
  const std::string& get_key() const { return m_scene_key; }

 protected:
  const std::string m_scene_key;
  const std::filesystem::path m_scene_dir;
  GameContext& m_game_context;
  LuaAPI m_lua;
  Renderer m_renderer{*m_game_context.asset_manager};
  Camera m_camera{*m_game_context.display};
  bool m_has_loaded = false;
};
}  // namespace dl
