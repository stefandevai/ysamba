#include "./label.hpp"

#include "graphics/renderer.hpp"

namespace dl::ui
{
Label::Label() : UIComponent(), text(Text()) {}

Label::Label(const std::string_view value) : UIComponent(), text(Text(value)) {}

void Label::render([[maybe_unused]] Renderer& renderer, Batch& batch)
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
  text.set_text(value);
  size = text.get_size();
}
}  // namespace dl::ui
