#include "./scene_manager.hpp"

#include <spdlog/spdlog.h>

#include <memory>

#include "./game_context.hpp"
#include "core/input_manager.hpp"

namespace dl
{
void SceneManager::pop_scene()
{
  if (m_scenes.empty())
  {
    spdlog::warn("Scene stack is empty");
    return;
  }

  InputManager::get_instance()->pop_context();
  m_scenes.pop_back();
}

void SceneManager::update(GameContext& game_context)
{
  if (m_scenes.empty())
  {
    return;
  }

  auto& current_scene = m_scenes.back();

  if (!current_scene->has_loaded())
  {
    current_scene->load();
  }

  current_scene->update(game_context);
}

void SceneManager::render()
{
  if (m_scenes.empty())
  {
    return;
  }

  auto& current_scene = m_scenes.back();

  if (!current_scene->has_loaded())
  {
    return;
  }
  current_scene->check_window_size();
  current_scene->render();
  current_scene->render_call();
}

}  // namespace dl
