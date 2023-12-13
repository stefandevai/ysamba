#include "./label.hpp"

#include "graphics/renderer.hpp"

namespace dl::ui
{
Label::Label() : UIComponent(), text(Text()) {}

Label::Label(const std::string_view value) : UIComponent(), text(Text(value)) {}

void Label::set_text(const std::string_view value)
{
  text.set_text(value);
  size = text.get_size();
}

void Label::render([[maybe_unused]] Renderer& renderer, Batch& batch)
{
  batch.text(text, absolute_position.x, absolute_position.y, absolute_position.z);
}
}  // namespace dl::ui
