#include "./label.hpp"

#include "graphics/renderer.hpp"

namespace dl::ui
{
Label::Label(const std::string& text, const std::string& typeface) : UIComponent(), text(Text(text, typeface)) {}
void Label::render(Renderer& renderer, const std::string& layer)
{
  renderer.get_layer(layer)->text(text, absolute_position.x, absolute_position.y, absolute_position.z);
}
}  // namespace dl::ui
