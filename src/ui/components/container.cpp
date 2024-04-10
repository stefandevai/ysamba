#include "./container.hpp"

#include "core/maths/vector.hpp"
#include "graphics/renderer/renderer.hpp"

namespace dl::ui
{
Container::Container(UIContext& context, Params params)
    : UIComponent(context, "Container"), quad(Quad{params.size.x, params.size.y, Color{params.color}})
{
  is_renderable = true;
  size = std::move(params.size);
}

void Container::render()
{
  if (!is_active())
  {
    return;
  }

  if (quad.color.opacity_factor != opacity)
  {
    quad.color.opacity_factor = opacity;
  }

  m_context.renderer->ui_pass.batch.quad(quad, absolute_position.x, absolute_position.y, absolute_position.z);

  for (auto& child : children)
  {
    child->render();
  }
}

void Container::set_size(const Vector2i size)
{
  this->size = std::move(size);
  quad.w = size.x;
  quad.h = size.y;
}

void Container::set_color(const uint32_t color)
{
  quad.color.set(color);
}
}  // namespace dl::ui
