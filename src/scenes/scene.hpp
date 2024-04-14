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
  virtual ~Scene() = default;

  // Called only once when the scene is pushed to the stack
  virtual void load() = 0;

  // Called once when the scene is added to the stack and during scene transitions
  virtual void init() {}

  // Called every frame while the scene is active
  virtual void update() = 0;

  // Called every frame while the scene is active
  virtual void render() = 0;

  // Handle window resize events
  void resize();

  // Check if the scene has been loaded
  [[nodiscard]] bool has_loaded() const { return m_has_loaded; }

  // Gets the scene key
  [[nodiscard]] uint32_t get_key() const { return m_scene_key; }

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
