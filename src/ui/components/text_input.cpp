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
  m_label = m_container->emplace<Label>("");
  m_label->wrap = false;
}

void TextInput::init()
{ /* m_label->set_text(text); */
}

void TextInput::update()
{
  using namespace entt::literals;

  if (m_state == State::Focus)
  {
    const auto& text_input = m_input_manager.get_text_input();

    if (text_input != text)
    {
      m_label->set_text(text_input);
      text = text_input;
    }
  }

  if (m_input_manager.has_clicked_bounds(
          InputManager::MouseButton::Left, {absolute_position.x, absolute_position.y}, size))
  {
    if (m_state == State::Display)
    {
      m_state = State::Focus;
      m_input_manager.push_context("text_input"_hs);
      m_input_manager.text_input_start();
    }
  }
  else if (m_input_manager.has_clicked(InputManager::MouseButton::Left))
  {
    if (m_state == State::Focus)
    {
      m_input_manager.text_input_stop();
      m_input_manager.pop_context();
      m_state = State::Display;
    }
  }
  else if (m_input_manager.is_context("text_input"_hs) && m_input_manager.poll_action("quit"_hs))
  {
    m_input_manager.text_input_stop();
    m_input_manager.pop_context();
    m_state = State::Display;
  }
}

}  // namespace dl::ui
