#include "./container.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/maths/vector.hpp"
#include "graphics/renderer.hpp"

namespace dl::ui
{
Container::Container() : UIComponent(), quad(std::make_unique<Quad>(0, 0, Color{"#ffffffff"})) {}

Container::Container(const Vector2i& size, const std::string& color)
    : UIComponent(), quad(std::make_unique<Quad>(size.x, size.y, Color{color}))
{
  this->size = size;
}

void Container::render(Renderer& renderer, const std::string& layer)
{
  renderer.get_layer(layer)->quad(quad, absolute_position.x, absolute_position.y, absolute_position.z);

  for (auto& child : children)
  {
    if (child.expired())
    {
      continue;
    }

    auto child_ptr = child.lock();
    child_ptr->render(renderer, layer);
  }
}

void Container::set_size(const Vector2i& size)
{
  this->size = size;
  this->quad->w = size.x;
  this->quad->h = size.y;
}

void Container::set_color(const std::string& color) { quad->color = Color{color}; }
}  // namespace dl::ui
