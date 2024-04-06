#include "./container.hpp"

#include "core/maths/vector.hpp"
#include "graphics/renderer/renderer.hpp"

namespace dl::ui
{
Container::Container(UIContext& context) : UIComponent(context), quad(std::make_unique<Quad>(0, 0, Color{0xffffffff}))
{
}

Container::Container(UIContext& context, const Vector2i& size, const uint32_t color)
    : UIComponent(context), quad(std::make_unique<Quad>(size.x, size.y, Color{color}))
{
  this->size = size;
}

void Container::render()
{
  if (!is_active())
  {
    return;
  }

  if (quad->color.opacity_factor != opacity)
  {
    quad->color.opacity_factor = opacity;
  }

  m_context.renderer->ui_pass.batch.quad(quad.get(), absolute_position.x, absolute_position.y, absolute_position.z);

  for (auto& child : children)
  {
    child->render();
  }
}

void Container::set_size(const Vector2i& size)
{
  this->size = size;
  quad->w = size.x;
  quad->h = size.y;
}

void Container::set_color(const uint32_t color)
{
  quad->color = Color{color};
}
}  // namespace dl::ui
