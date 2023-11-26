#include "./scene.hpp"

#include <SDL.h>

#include "core/display.hpp"
#include "core/input_manager.hpp"

namespace dl
{
Scene::Scene(const std::string& scene_key, GameContext& game_context)
    : m_scene_key(scene_key), m_scene_dir("scenes/" + scene_key + "/"), m_game_context(game_context)
{
}

Scene::~Scene() {}

void Scene::load() { m_lua.load(m_scene_dir / "main.lua"); }

void Scene::check_window_size()
{
  const auto input_manager = InputManager::get_instance();

  if (input_manager->window_size_changed())
  {
    const auto& display_size = m_game_context.display->get_window_size();

    m_game_context.display->update_viewport();
    m_camera.set_size({static_cast<double>(display_size.x), static_cast<double>(display_size.y)});
    input_manager->set_window_size_changed(false);
  }
}
}  // namespace dl
