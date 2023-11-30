#include "./container.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/maths/vector.hpp"
#include "graphics/batch.hpp"

namespace dl::ui
{
Container::Container() : UIComponent(), quad(std::make_shared<Quad>(0, 0, Color{"#ffffffff"})) {}

Container::Container(const Vector2i& size, const std::string& color)
    : UIComponent(), quad(std::make_shared<Quad>(size.x, size.y, Color{color}))
{
  this->size = size;
}

void Container::render(const std::shared_ptr<Batch> batch)
{
  batch->quad(quad, absolute_position.x, absolute_position.y, absolute_position.z);

  for (auto& child : children)
  {
    child->render(batch);
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
