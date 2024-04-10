#include "./ui_manager.hpp"

#include <spdlog/spdlog.h>

#include "core/asset_manager.hpp"
#include "core/input_manager.hpp"
#include "graphics/display.hpp"
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

  // Process input
  if (m_focused_stack.empty())
  {
    for (auto& component : m_components)
    {
      component->m_process_input();
    }
  }
  else
  {
    m_focused_stack.back()->m_process_input();
  }

  // Update animations
  m_animation_manager.update(m_clock.delta);

  // Update components
  // Reset min/max z_index so that we get the updated values each frame
  m_context.min_z_index = 0;
  m_context.max_z_index = 0;

  for (auto& component : m_components)
  {
    if (component->state == UIComponent::State::Hidden)
    {
      continue;
    }

    component->m_update();
  }

  // Update notifications
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
            [](const auto& a, const auto& b) { return a->absolute_position.z < b->absolute_position.z; });

  for (auto& component : m_components)
  {
    if (component->is_hidden())
    {
      continue;
    }

    // Push scissor during animation to resolve issues with components stacking on top of other components
    // that contain a scissor
    if (component->state == UIComponent::State::Animating)
    {
      const auto& window_size = Display::get_window_size();
      const int scissor_x = std::clamp(component->absolute_position.x, 0, window_size.x);
      const int scissor_y = std::clamp(component->absolute_position.y, 0, window_size.y);
      int scissor_width = component->size.x;
      int scissor_height = component->size.y;

      if (scissor_x + scissor_width > window_size.x)
      {
        scissor_width = window_size.x - scissor_x;
      }
      if (scissor_y + scissor_height > window_size.y)
      {
        scissor_height = window_size.y - scissor_y;
      }

      m_context.renderer->ui_pass.batch.push_scissor({scissor_x, scissor_y, scissor_width, scissor_height});
    }

    component->render();

    if (component->state == UIComponent::State::Animating)
    {
      m_context.renderer->ui_pass.batch.pop_scissor();
    }
  }

  for (auto& notification : m_notifications)
  {
    if (notification->is_hidden())
    {
      continue;
    }

    // Push scissor during animation to resolve issues with components stacking on top of other components
    // that contain a scissor
    if (notification->state == UIComponent::State::Animating)
    {
      const auto& window_size = Display::get_window_size();
      const int scissor_x = std::clamp(notification->absolute_position.x, 0, window_size.x);
      const int scissor_y = std::clamp(notification->absolute_position.y, 0, window_size.y);
      int scissor_width = notification->size.x;
      int scissor_height = notification->size.y;

      if (scissor_x + scissor_width > window_size.x)
      {
        scissor_width = window_size.x - scissor_x;
      }
      if (scissor_y + scissor_height > window_size.y)
      {
        scissor_height = window_size.y - scissor_y;
      }

      m_context.renderer->ui_pass.batch.push_scissor({scissor_x, scissor_y, scissor_width, scissor_height});
    }

    notification->render();

    if (notification->state == UIComponent::State::Animating)
    {
      m_context.renderer->ui_pass.batch.pop_scissor();
    }
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
