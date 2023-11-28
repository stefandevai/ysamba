#include "./button.hpp"

#include "graphics/batch.hpp"

namespace dl::ui
{
Button::Button() : UIComponent(ComponentType::Quad)
{
  // TODO: Create continer and label components
  // Add the to children vector
}

void Button::render(Batch& batch) {}

}  // namespace dl::ui
