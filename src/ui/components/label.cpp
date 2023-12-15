#include "./label.hpp"

#include "graphics/renderer.hpp"

namespace dl::ui
{
Label::Label(UIContext& context) : UIComponent(context), text(Text()) {}

Label::Label(UIContext& context, const std::string_view value) : UIComponent(context), text(Text(value)) {}

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
  text.set_text(value);
  size = text.get_size();
}
}  // namespace dl::ui
