#include "./text_input.hpp"

#include <spdlog/spdlog.h>

#include "ui/components/container.hpp"
#include "ui/components/label.hpp"
#include "ui/components/nine_patch_container.hpp"

namespace dl::ui
{
TextInput::TextInput(UIContext& context, Params params)
    : UIComponent(context, "TextInput"), text(std::move(params.text)), placeholder(std::move(params.placeholder))
{
  size = std::move(params.size);

  container = emplace<NinePatchContainer>(NinePatchContainer::Params{
      .id = 3,
      .size = size,
      .color = 0x7EB1A1FF,
  });

  label = container->emplace<Label>(Label::Params{
      .value = text.empty() ? placeholder : text,
      .wrap = false,
  });
  label->margin = Vector2i{12, 0};
  label->y_alignment = YAlignement::Center;

  const int cursor_height = label->text.font_size;
  const uint32_t cursor_color = label->text.color.int_color;

  cursor = container->emplace<Container>(Container::Params{
      .size = Vector2i{1, cursor_height},
      .color = cursor_color,
  });
  cursor->margin = label->margin;
  cursor->y_alignment = YAlignement::Center;
  cursor->position = Vector3i{-1, -1, 0};
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
      label->set_text(text_input);
      label->init();
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
    auto cursor_position = label->text.get_position_at(cursor_index);

    if (cursor_position.x == -1 && cursor_position.y == -1)
    {
      cursor_position.x = 0;
      cursor_position.y = 0;
    }

    if (cursor_position.x != cursor->position.x || cursor_position.y != cursor->position.y)
    {
      cursor->position.x = cursor_position.x;
      cursor->position.y = cursor_position.y;
      dirty = true;
    }

    // Toggle cursor display state
    if (m_cursor_timer.elapsed<std::chrono::milliseconds>() >= 500)
    {
      m_cursor_state = m_cursor_state == CursorState::Display ? CursorState::Hide : CursorState::Display;
      m_cursor_timer.start();
    }

    // Blink cursor
    if (m_cursor_state == CursorState::Hide && cursor->state != State::Hidden)
    {
      cursor->hide();
    }
    else if (m_cursor_state == CursorState::Display && cursor->state != State::Visible)
    {
      cursor->show();
    }
  }

  // Hide cursor when not focused
  if (m_state != InputState::Focus && cursor->state == State::Visible)
  {
    cursor->hide();
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
      if (text != label->value)
      {
        label->set_text(text);
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

      if (text.empty())
      {
        label->set_text(placeholder);
      }

      m_state = InputState::Display;
    }
  }
  else if (m_input_manager.is_context("text_input"_hs) && m_input_manager.poll_action("quit"_hs))
  {
    m_input_manager.text_input_stop();
    m_input_manager.pop_context();

    if (text.empty())
    {
      label->set_text(placeholder);
    }

    m_state = InputState::Display;
  }
}

}  // namespace dl::ui
