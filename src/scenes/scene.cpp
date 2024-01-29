#include "./scene.hpp"

#include <SDL.h>
#include <fmt/format.h>

#include <entt/core/hashed_string.hpp>

#include "core/display.hpp"
#include "core/input_manager.hpp"

namespace dl
{
Scene::Scene(const std::string& scene_key, GameContext& game_context)
    : m_scene_key(entt::hashed_string{scene_key.c_str()}),
      m_scene_path(fmt::format("data/scenes/{}/data.json", scene_key)),
      m_game_context(game_context)
{
  resize();
}

void Scene::load() { m_json.load(m_scene_path); }

void Scene::resize()
{
  const auto& display_size = Display::get_window_size();
  m_camera.set_size({static_cast<double>(display_size.x), static_cast<double>(display_size.y)});
}
}  // namespace dl
