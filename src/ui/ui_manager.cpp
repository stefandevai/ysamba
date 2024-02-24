#include "./ui_manager.hpp"

#include <spdlog/spdlog.h>

#include "./components/container.hpp"
#include "./components/label.hpp"
#include "core/asset_manager.hpp"
// #include "graphics/batch.hpp"
#include "graphics/renderer/renderer.hpp"

namespace dl::ui
{
UIManager::UIManager(AssetManager* asset_manager, Renderer* renderer)
    : m_asset_manager(asset_manager), m_renderer(renderer)
{
  // Push identity matrix as stack base
  m_matrix_stack.push_back(glm::mat4(1));
}

UIManager::~UIManager() { m_matrix_stack.clear(); }

void UIManager::update()
{
  m_clock.tick();
  m_animation_manager.update(m_clock.delta);

  for (auto& component : m_components)
  {
    if (component->state == UIComponent::State::Hidden)
    {
      continue;
    }

    component->m_update();
  }
}

void UIManager::render()
{
  for (auto& component : m_components)
  {
    if (component->state == UIComponent::State::Hidden)
    {
      continue;
    }

    component->render();
  }
}

void UIManager::force_hide_all()
{
  for (auto& component : m_components)
  {
    if (component->state != UIComponent::State::Hidden)
    {
      component->state = UIComponent::State::Hidden;
    }
  }
}

}  // namespace dl::ui
