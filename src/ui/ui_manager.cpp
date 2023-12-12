#include "./ui_manager.hpp"

#include <spdlog/spdlog.h>

#include "./components/container.hpp"
#include "./components/label.hpp"
#include "graphics/batch.hpp"
#include "graphics/renderer.hpp"

namespace dl::ui
{
UIManager::UIManager()
{
  m_batch.has_depth = false;
  m_matrix_stack.push_back(glm::mat4(1));
}

UIManager::~UIManager() { m_matrix_stack.clear(); }

void UIManager::update()
{
  for (auto& component : m_components)
  {
    if (!component->visible)
    {
      continue;
    }

    component->update_component(m_matrix_stack);
  }
}

void UIManager::render(Renderer& renderer)
{
  if (!m_added_batch)
  {
    renderer.add_batch(&m_batch);
    m_added_batch = true;
  }

  for (auto& component : m_components)
  {
    if (!component->visible)
    {
      continue;
    }

    component->render(renderer, m_batch);
  }
}

}  // namespace dl::ui
