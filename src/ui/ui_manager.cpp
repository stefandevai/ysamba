#include "./ui_manager.hpp"

#include <spdlog/spdlog.h>

#include "./components/container.hpp"
#include "./components/label.hpp"
#include "graphics/batch.hpp"
#include "graphics/renderer.hpp"

namespace dl::ui
{
UIManager::UIManager() { m_matrix_stack.push_back(glm::mat4(1)); }

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

void UIManager::remove_component(const uint32_t id) { m_components.erase(id); }

void UIManager::update()
{
  for (auto& c : m_components)
  {
    c.second->update(m_matrix_stack);
  }
}

void UIManager::render(std::shared_ptr<Batch> batch)
{
  if (batch == nullptr)
  {
    spdlog::warn("No batch loaded in UI Manager");
    return;
  }

  for (auto& c : m_components)
  {
    c.second->render(batch);
  }
}

}  // namespace dl::ui
