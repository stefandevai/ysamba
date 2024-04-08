#include "./ui_manager.hpp"

#include <spdlog/spdlog.h>

#include "core/asset_manager.hpp"
#include "core/input_manager.hpp"
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
  m_context.min_z_index = 0;
  m_context.max_z_index = 0;

  if (m_focused_stack.empty())
  {
    for (auto& component : m_components)
    {
      if (component->state == UIComponent::State::Hidden)
      {
        continue;
      }

      component->m_process_input();
    }
  }
  else
  {
    m_focused_stack.back()->m_process_input();
  }

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
  // Sort top level components by z-index
  std::sort(m_components.begin(),
            m_components.end(),
            [](const auto& a, const auto& b) { return a->position.z < b->position.z; });

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

void UIManager::bring_to_front(UIComponent* component)
{
  ++m_context.max_z_index;
  component->position.z = m_context.max_z_index;
  component->dirty = true;
}

}  // namespace dl::ui
