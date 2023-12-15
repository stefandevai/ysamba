#include "./label.hpp"

#include <spdlog/spdlog.h>

#include "core/display.hpp"
#include "graphics/renderer.hpp"

namespace dl::ui
{
Label::Label(UIContext& context) : UIComponent(context) {}

Label::Label(UIContext& context, const std::string_view value) : UIComponent(context), m_value(value) {}

void Label::init()
{
  if (m_value.empty())
  {
    return;
  }

  if (wrap)
  {
    const auto& window_size = Display::get_window_size();
    const auto wrap_width =
        (parent == nullptr || parent->size.x == 0) ? window_size.x - 2 * margin.x : parent->size.x - 2 * margin.x;
    text.set_text_wrapped(m_value, wrap_width);
  }
  else
  {
    text.set_text(m_value);
  }

  size = text.get_size();
}

void Label::render(Batch& batch)
{
  if (state == State::Hidden)
  {
    return;
  }

  if (text.color.opacity_factor != opacity)
  {
    text.color.opacity_factor = opacity;
  }

  batch.text(text, absolute_position.x, absolute_position.y, absolute_position.z);
}

void Label::set_text(const std::string_view value)
{
  m_value = value;
  has_initialized = false;
}
}  // namespace dl::ui
