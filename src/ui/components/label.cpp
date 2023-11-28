#include "./label.hpp"

#include "graphics/batch.hpp"

namespace dl::ui
{
Label::Label(const std::string& text, const std::string& typeface)
    : UIComponent(ComponentType::Text), text(Text(text, typeface))
{
}
void Label::render(const std::shared_ptr<Batch> batch)
{
  batch->text(text, absolute_position.x, absolute_position.y, absolute_position.z);
}
}  // namespace dl::ui
