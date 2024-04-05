#include "./ui_manager.hpp"

#include <spdlog/spdlog.h>

#include "core/asset_manager.hpp"
#include "graphics/renderer/renderer.hpp"

namespace dl::ui
{
UIManager::UIManager(AssetManager* asset_manager, Renderer* renderer)
    : m_asset_manager(asset_manager), m_renderer(renderer)
{
  // Push identity matrix as stack base
  m_matrix_stack.push_back(glm::mat4(1));
}

UIManager::~UIManager()
{
  m_matrix_stack.clear();
}

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

  for (auto& notification : m_notifications)
  {
    if (notification->state == UIComponent::State::Hidden)
    {
      continue;
    }

    notification->m_update();
  }

  // Remove notifications after they are hidden
  std::erase_if(m_notifications, [](const auto& component) { return component->state == UIComponent::State::Hidden; });
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

  for (auto& notification : m_notifications)
  {
    if (notification->state == UIComponent::State::Hidden)
    {
      continue;
    }

    notification->render();
  }
}

Notification* UIManager::notify(const std::string& notification)
{
  auto component = std::make_unique<Notification>(m_context, notification);
  component->show();
  m_notifications.push_back(std::move(component));
  return &(*m_notifications.back());
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
