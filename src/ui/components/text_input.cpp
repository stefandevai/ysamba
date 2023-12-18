#include "./text_input.hpp"

#include <spdlog/spdlog.h>

#include "./container.hpp"
#include "./label.hpp"
#include "core/asset_manager.hpp"
#include "graphics/batch.hpp"

namespace dl::ui
{
TextInput::TextInput(UIContext& context) : UIComponent(context)
{
  size = {100, 32};
  m_container = emplace<Container>(size, 0xaa5533ff);
  m_label = m_container->emplace<Label>("Test");
}

void TextInput::init() { /* m_label->set_text(text); */ }

void TextInput::update()
{
  if (m_input_manager.has_clicked_bounds(
          InputManager::MouseButton::Left, {absolute_position.x, absolute_position.y}, size))
  {
  }
  else if (m_input_manager.has_clicked(InputManager::MouseButton::Left))
  {
  }
}

}  // namespace dl::ui
