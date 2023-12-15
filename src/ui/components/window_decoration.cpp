#include "./window_decoration.hpp"

#include "core/maths/vector.hpp"
#include "graphics/renderer.hpp"

namespace dl::ui
{
WindowDecoration::WindowDecoration(UIContext& context)
    : UIComponent(context), quad(std::make_unique<Quad>(0, 0, Color{0xffffffff}))
{
}

void WindowDecoration::render(Batch& batch)
{
  if (state == State::Hidden)
  {
    return;
  }

  if (quad->color.opacity_factor != opacity)
  {
    quad->color.opacity_factor = opacity;
  }

  batch.quad(quad.get(), absolute_position.x, absolute_position.y, absolute_position.z);

  for (auto& child : children)
  {
    child->render(batch);
  }
}

void WindowDecoration::set_size(const Vector2i& size)
{
  this->size = size;
  quad->w = size.x;
  quad->h = size.y;
}

void WindowDecoration::set_color(const uint32_t color) { quad->color = Color{color}; }
}  // namespace dl::ui
