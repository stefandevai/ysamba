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

UIManager::~UIManager()
{
  m_matrix_stack.clear();
  m_components.clear();
}

uint32_t UIManager::add_component(const std::shared_ptr<UIComponent>& component)
{
  const auto id = m_identifier();
  m_components.insert({id, component});
  return id;
}

void UIManager::erase(const UIComponent& component) {}

void UIManager::remove_component(const uint32_t id) { m_components.erase(id); }

void UIManager::update()
{
  for (auto& component : m_components2)
  {
    if (!component->visible)
    {
      continue;
    }

    component->update_component(m_matrix_stack);
  }

  /* for (auto& c : m_components) */
  /* { */
  /*   if (c.second.expired()) */
  /*   { */
  /*     continue; */
  /*   } */

  /*   const auto& c_ptr = c.second.lock(); */

  /*   if (!c_ptr->visible) */
  /*   { */
  /*     continue; */
  /*   } */

  /*   c_ptr->update_component(m_matrix_stack); */
  /* } */
}

void UIManager::render(Renderer& renderer)
{
  if (!m_added_batch)
  {
    renderer.add_batch(&m_batch);
    m_added_batch = true;
  }

  for (auto& component : m_components2)
  {
    if (!component->visible)
    {
      continue;
    }

    component->render(renderer, m_batch);
  }

  /* for (auto& c : m_components) */
  /* { */
  /*   if (c.second.expired()) */
  /*   { */
  /*     continue; */
  /*   } */

  /*   const auto& c_ptr = c.second.lock(); */

  /*   if (!c_ptr->visible) */
  /*   { */
  /*     continue; */
  /*   } */

  /*   c_ptr->render(renderer, m_batch); */
  /* } */
}

}  // namespace dl::ui
