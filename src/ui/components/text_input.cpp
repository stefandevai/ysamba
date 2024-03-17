#include "./text_input.hpp"

#include <spdlog/spdlog.h>

#include "./container.hpp"
#include "./label.hpp"
#include "graphics/renderer/renderer.hpp"

namespace dl::ui
{
TextInput::TextInput(UIContext& context) : UIComponent(context)
{
  size = Vector2i{160, 32};
  m_container = emplace<Container>(size, 0x2f4241ff);
  m_label = m_container->emplace<Label>("");
  m_label->wrap = false;
  m_label->margin = Vector2i{8, 0};
  m_label->y_alignment = YAlignement::Center;
  m_cursor.color = m_label->text.color;
  m_cursor.h = m_label->text.font_size;
}

void TextInput::init()
{
  m_container->set_size(size);

  if (text.empty())
  {
    m_label->set_text(placeholder);
  }
  else
  {
    m_label->set_text(text);
  }
}

void TextInput::update()
{
  using namespace entt::literals;

  if (m_state == InputState::Focus)
  {
    const auto& text_input = m_input_manager.get_text_input();

    if (text_input != text)
    {
      m_label->set_text(text_input);
      text = text_input;
      m_cursor_timer.start();
      m_cursor_state = CursorState::Display;
    }

    const auto new_cursor_index = m_input_manager.get_text_input_cursor_index();

    if (new_cursor_index != cursor_index)
    {
      cursor_index = new_cursor_index;
      m_cursor_timer.start();
      m_cursor_state = CursorState::Display;
    }

    if (m_cursor_timer.elapsed<std::chrono::milliseconds>() >= 500)
    {
      m_cursor_state = m_cursor_state == CursorState::Display ? CursorState::Hide : CursorState::Display;
      m_cursor_timer.start();
    }
  }

  if (m_input_manager.has_clicked_bounds(
          InputManager::MouseButton::Left, {absolute_position.x, absolute_position.y}, size))
  {
    if (m_state == InputState::Display)
    {
      m_state = InputState::Focus;
      m_cursor_state = CursorState::Display;
      m_cursor_timer.start();

      // Replace placeholder with actual text value
      if (text != m_label->value)
      {
        m_label->set_text(text);
      }

      m_input_manager.push_context("text_input"_hs);
      m_input_manager.set_text_input(text);
      m_input_manager.text_input_start();
    }
  }
  else if (m_input_manager.has_clicked(InputManager::MouseButton::Left))
  {
    if (m_state == InputState::Focus)
    {
      m_input_manager.text_input_stop();
      m_input_manager.pop_context();
      m_state = InputState::Display;
    }
  }
  else if (m_input_manager.is_context("text_input"_hs) && m_input_manager.poll_action("quit"_hs))
  {
    m_input_manager.text_input_stop();
    m_input_manager.pop_context();
    m_state = InputState::Display;
  }
}

void TextInput::render()
{
  if (state == State::Hidden)
  {
    return;
  }

  for (auto& child : children)
  {
    child->render();
  }

  // Render cursor
  if (m_state == InputState::Focus && m_cursor_state == CursorState::Display)
  {
    if (m_cursor.color.opacity_factor != opacity)
    {
      m_cursor.color.opacity_factor = opacity;
    }

    auto cursor_position = m_label->text.get_position_at(cursor_index);

    if (cursor_position.x == -1 && cursor_position.y == -1)
    {
      cursor_position.x = 0;
      cursor_position.y = 0;
    }

    // TODO: Automatically center cursor vertically
    cursor_position.y += 8;

    m_context.renderer->ui_pass.batch.quad(&m_cursor,
                                           absolute_position.x + m_label->margin.x + cursor_position.x,
                                           absolute_position.y + m_label->margin.y + cursor_position.y,
                                           absolute_position.z);
  }
}

}  // namespace dl::ui
