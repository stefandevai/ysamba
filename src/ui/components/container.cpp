#include "./container.hpp"

#include "core/maths/vector.hpp"
#include "graphics/renderer.hpp"

namespace dl::ui
{
Container::Container() : UIComponent(), quad(std::make_unique<Quad>(0, 0, Color{0xffffffff})) {}

Container::Container(const Vector2i& size, const uint32_t color)
    : UIComponent(size), quad(std::make_unique<Quad>(size.x, size.y, Color{color}))
{
}

void Container::render(Renderer& renderer, Batch& batch)
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
    child->render(renderer, batch);
  }
}

void Container::set_size(const Vector2i& size)
{
  this->size = size;
  quad->w = size.x;
  quad->h = size.y;
}

void Container::set_color(const uint32_t color) { quad->color = Color{color}; }
}  // namespace dl::ui
