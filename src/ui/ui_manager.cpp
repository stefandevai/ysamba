#include "./ui_manager.hpp"

#include <spdlog/spdlog.h>

#include "./components/container.hpp"
#include "./components/label.hpp"
#include "core/asset_manager.hpp"
#include "graphics/batch.hpp"
#include "graphics/renderer.hpp"

namespace dl::ui
{
UIManager::UIManager(AssetManager* asset_manager, Renderer* renderer)
    : m_asset_manager(asset_manager), m_renderer(renderer)
{
  m_batch.has_depth = false;
  // TODO: Add callback in input manager to handle this whenever the window size changes
  glm::mat4 projection_matrix = glm::ortho(0.0f, 1280.0f, 755.0f, 0.0f, 0.1f, 1000.0f);
  glm::mat4 default_view_matrix =
      glm::lookAt(glm::vec3{0.f, 0.f, 500.f}, glm::vec3{0.f, 0.f, -1.f}, glm::vec3{0.0f, 1.0f, 0.0f});
  m_batch.push_matrix(projection_matrix * default_view_matrix);
  m_matrix_stack.push_back(glm::mat4(1));
  m_renderer->add_batch(&m_batch);
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

    component->render(m_batch);
  }
}

}  // namespace dl::ui
