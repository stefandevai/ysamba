#include "./scene.hpp"

#include <SDL.h>

#include <entt/core/hashed_string.hpp>

#include "core/display.hpp"
#include "core/input_manager.hpp"

namespace dl
{
Scene::Scene(const std::string& scene_key, GameContext& game_context)
    : m_scene_key(entt::hashed_string{scene_key.c_str()}), m_scene_key_string(scene_key), m_game_context(game_context)
{
  const auto& display_size = Display::get_window_size();
  m_renderer.set_projection_matrix(m_camera.get_projection_matrix());
  m_camera.set_size({static_cast<double>(display_size.x), static_cast<double>(display_size.y)});
}

void Scene::load() { m_json.load("./data/scenes/" + m_scene_key_string + "/data.json"); }

void Scene::check_window_size()
{
  auto& input_manager = InputManager::get_instance();

  if (input_manager.window_size_changed())
  {
    m_game_context.display->update_viewport();

    const auto& display_size = Display::get_window_size();
    m_camera.set_size({static_cast<double>(display_size.x), static_cast<double>(display_size.y)});
    m_renderer.set_projection_matrix(m_camera.get_projection_matrix());
    input_manager.set_window_size_changed(false);
  }
}
}  // namespace dl
