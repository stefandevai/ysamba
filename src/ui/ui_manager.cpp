#include "./ui_manager.hpp"

#include "./components/container.hpp"
#include "./components/label.hpp"
#include "graphics/renderer.hpp"

namespace dl::ui
{
UIManager::UIManager()
{
  /* m_sprite_batch.shader = dynamic_pointer_cast<ShaderProgram>(m_text_shader.construct()); */
  /* m_quad_batch.shader = dynamic_pointer_cast<ShaderProgram>(m_gui_shader.construct()); */

  /* m_sprite_batch.load(); */
  /* m_quad_batch.load(); */
}

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
  renderer.disable_depth_test();
  renderer.init("ui-text");

  for (auto& c : m_components)
  {
    auto& component = c.second;

    if (component->type == ComponentType::Text)
    {
      auto label = dynamic_pointer_cast<Label>(component);
      renderer.batch("ui-text", label->text, label->position.x, label->position.y, 0);
    }
  }

  renderer.finalize("ui-text");

  renderer.init("ui-quad");

  for (auto& c : m_components)
  {
    auto& component = c.second;

    if (component->type == ComponentType::Quad)
    {
      auto container = dynamic_pointer_cast<Container>(component);
      renderer.batch("ui-quad", container->quad, container->position.x, container->position.y, 0);
    }
  }

  renderer.finalize("ui-quad");
}

}  // namespace dl::ui
