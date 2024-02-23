#include "./label.hpp"

#include <spdlog/spdlog.h>

#include "graphics/display.hpp"
#include "graphics/renderer/renderer.hpp"

namespace dl::ui
{
Label::Label(UIContext& context) : UIComponent(context) {}

Label::Label(UIContext& context, const std::string_view value) : UIComponent(context), value(value) {}

void Label::init()
{
  if (wrap)
  {
    const auto& window_size = Display::get_window_size();
    const auto wrap_width
        = (parent == nullptr || parent->size.x == 0) ? window_size.x - 2 * margin.x : parent->size.x - 2 * margin.x;
    text.set_text_wrapped(value, wrap_width);
  }
  else
  {
    text.set_text(value);
  }

  size = text.get_size();
}

void Label::render()
{
  if (state == State::Hidden)
  {
    return;
  }

  if (text.color.opacity_factor != opacity)
  {
    text.color.opacity_factor = opacity;
  }

  m_context.renderer->ui_pass.batch.text(text, absolute_position.x, absolute_position.y, absolute_position.z + 0.2f);
}

void Label::set_text(const std::string_view value)
{
  if (this->value == value)
  {
    return;
  }

  this->value = value;
  has_initialized = false;
}
}  // namespace dl::ui
