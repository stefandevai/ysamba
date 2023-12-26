#pragma once

#include <string>

#include "core/game_context.hpp"
#include "core/json.hpp"
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
  virtual ~Scene(){};

  virtual void load();
  virtual void update() = 0;
  virtual void render() = 0;
  void resize();
  void render_call() { m_renderer.render(m_camera.projection_matrix); }
  void check_window_size();
  bool has_loaded() const { return m_has_loaded; }
  uint32_t get_key() const { return m_scene_key; }

 protected:
  const uint32_t m_scene_key{};
  const std::string m_scene_key_string{};
  GameContext& m_game_context;
  JSON m_json{};
  Renderer m_renderer{*m_game_context.asset_manager};
  Camera m_camera{*m_game_context.display};
  bool m_has_loaded = false;
};
}  // namespace dl
