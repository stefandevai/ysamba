#include "./label.hpp"

#include "graphics/renderer.hpp"

namespace dl::ui
{
Label::Label(const std::string& text, const std::string& typeface) : UIComponent(), text(Text(text, typeface))
{
  size = this->text.get_size();
}
void Label::render(Renderer& renderer, Batch& batch)
{
  (void)renderer;
  batch.text(text, absolute_position.x, absolute_position.y, absolute_position.z);
}
}  // namespace dl::ui
