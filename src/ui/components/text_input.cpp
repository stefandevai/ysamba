#include "./text_input.hpp"

#include <spdlog/spdlog.h>

#include "./container.hpp"
#include "./label.hpp"
#include "graphics/renderer/renderer.hpp"

namespace dl::ui
{
TextInput::TextInput(UIContext& context, Params params)
    : UIComponent(context, "TextInput"), text(std::move(params.text)), placeholder(std::move(params.placeholder))
{
  size = std::move(params.size);

  m_container = emplace<Container>(Container::Params{
      .size = size,
      .color = 0x2f4241ff,
  });

  m_label = m_container->emplace<Label>(Label::Params{
      .value = text.empty() ? placeholder : text,
      .wrap = false,
  });
  m_label->margin = Vector2i{8, 0};
  m_label->y_alignment = YAlignement::Center;

  const int cursor_height = m_label->text.font_size;
  const uint32_t cursor_color = m_label->text.color.int_color;

  m_cursor = m_container->emplace<Container>(Container::Params{
      .size = Vector2i{1, cursor_height},
      .color = cursor_color,
  });
  m_cursor->margin = m_label->margin;
  m_cursor->y_alignment = YAlignement::Center;
  m_cursor->position = Vector3i{-1, -1, 0};
}

void TextInput::update()
{
  using namespace entt::literals;

  if (m_state == InputState::Focus)
  {
    const auto& text_input = m_input_manager.get_text_input();

    if (text_input != text)
    {
      // Restart label and compute new characters in place so we can correctly get the new cursor position
      // if it's at the end of the string.
      m_label->set_text(text_input);
      m_label->init();
      text = text_input;
      m_cursor_timer.start();
      m_cursor_state = CursorState::Display;
    }

    // Update index
    const auto new_cursor_index = m_input_manager.get_text_input_cursor_index();

    if (new_cursor_index != cursor_index)
    {
      cursor_index = new_cursor_index;
      m_cursor_timer.start();
      m_cursor_state = CursorState::Display;
    }

    // Update cursor position
    auto cursor_position = m_label->text.get_position_at(cursor_index);

    if (cursor_position.x == -1 && cursor_position.y == -1)
    {
      cursor_position.x = 0;
      cursor_position.y = 0;
    }

    if (cursor_position.x != m_cursor->position.x || cursor_position.y != m_cursor->position.y)
    {
      m_cursor->position.x = cursor_position.x;
      m_cursor->position.y = cursor_position.y;
      dirty = true;
    }

    // Toggle cursor display state
    if (m_cursor_timer.elapsed<std::chrono::milliseconds>() >= 500)
    {
      m_cursor_state = m_cursor_state == CursorState::Display ? CursorState::Hide : CursorState::Display;
      m_cursor_timer.start();
    }

    // Blink cursor
    if (m_cursor_state == CursorState::Hide && m_cursor->state != State::Hidden)
    {
      m_cursor->hide();
    }
    else if (m_cursor_state == CursorState::Display && m_cursor->state != State::Visible)
    {
      m_cursor->show();
    }
  }

  // Hide cursor when not focused
  if (m_state != InputState::Focus && m_cursor->state == State::Visible)
  {
    m_cursor->hide();
  }

  // Show hand cursor when not focused and mouse is hovering
  if (m_state == InputState::Display && m_input_manager.mouse_hover_aabb(absolute_position.xy(), size))
  {
    m_input_manager.set_mouse_cursor(MouseCursor::Hand);
  }

  // Handle input click events
  if (m_input_manager.has_clicked_aabb(InputManager::MouseButton::Left, absolute_position.xy(), size))
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

}  // namespace dl::ui
