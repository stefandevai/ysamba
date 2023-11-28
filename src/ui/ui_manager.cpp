#include "./ui_manager.hpp"

#include <spdlog/spdlog.h>

#include "./components/container.hpp"
#include "./components/label.hpp"
#include "graphics/batch.hpp"
#include "graphics/renderer.hpp"

namespace dl::ui
{
UIManager::UIManager() {}

void UIManager::load(const std::shared_ptr<Batch>& batch) { m_batch = batch; }

uint32_t UIManager::add_component(const std::shared_ptr<UIComponent>& component)
{
  auto id = m_identifier();
  m_components[id] = component;
  return id;
}

void UIManager::remove_component(const uint32_t id) { m_components.erase(id); }

void UIManager::update() {}

void UIManager::render(Renderer& renderer)
{
  if (m_batch == nullptr)
  {
    spdlog::warn("No batch loaded in UI Manager");
    return;
  }

  renderer.disable_depth_test();

  for (auto& c : m_components)
  {
    auto& component = c.second;

    if (component->type == ComponentType::Text)
    {
      auto label = dynamic_pointer_cast<Label>(component);
      m_batch->text(label->text, label->position.x, label->position.y, 0);
    }
    else if (component->type == ComponentType::Quad)
    {
      auto container = dynamic_pointer_cast<Container>(component);
      m_batch->quad(container->quad, container->position.x, container->position.y, 0);
    }
  }
}

}  // namespace dl::ui
