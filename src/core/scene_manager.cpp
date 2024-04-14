#include "./scene_manager.hpp"

#include <spdlog/spdlog.h>

#include <memory>

namespace dl
{
void SceneManager::pop_scene()
{
  if (m_scenes.empty())
  {
    spdlog::warn("Scene stack is empty");
    return;
  }

  InputManager::get_instance().pop_context();
  m_scenes.pop_back();

  if (!m_scenes.empty())
  {
    auto& current_scene = m_scenes.back();
    current_scene->init();
  }
}

void SceneManager::update()
{
  if (m_scenes.empty())
  {
    return;
  }

  // Set arrow cursor by default each frame and update during loop if necessary
  InputManager::get_instance().set_mouse_cursor(MouseCursor::Arrow);

  auto& current_scene = m_scenes.back();

  if (!current_scene->has_loaded())
  {
    current_scene->load();
    current_scene->init();
  }

  current_scene->update();
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

  m_check_window_size();
  current_scene->render();
}

void SceneManager::m_check_window_size()
{
  auto& input_manager = InputManager::get_instance();

  if (input_manager.window_size_changed())
  {
    m_display.update_viewport();

    for (auto& scene : m_scenes)
    {
      scene->resize();
    }
  }
}

}  // namespace dl
