#include "./label.hpp"

#include "graphics/batch.hpp"

namespace dl::ui
{
void Label::render(Batch& batch) { batch.text(text, position.x, position.y, position.z); }
}  // namespace dl::ui
