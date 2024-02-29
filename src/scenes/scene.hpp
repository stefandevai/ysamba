#pragma once

#include <entt/entity/registry.hpp>
#include <string>

#include "core/game_context.hpp"
#include "graphics/camera.hpp"
#include "graphics/renderer/renderer.hpp"
#include "ui/ui_manager.hpp"

namespace dl
{
class Renderer;
struct GameContext;

class Scene
{
 public:
  Scene(const std::string& scene_key, GameContext& game_context);
  virtual ~Scene(){};

  virtual void load() = 0;
  virtual void update() = 0;
  virtual void render() = 0;
  void resize();
  void check_window_size();
  bool has_loaded() const { return m_has_loaded; }
  uint32_t get_key() const { return m_scene_key; }

 protected:
  const uint32_t m_scene_key{};
  const std::string m_scene_path{};
  GameContext& m_game_context;
  Renderer m_renderer{m_game_context};
  Camera m_camera{*m_game_context.display};
  ui::UIManager m_ui_manager{m_game_context.asset_manager, &m_renderer};
  entt::registry m_registry{};
  bool m_has_loaded = false;
};
}  // namespace dl
